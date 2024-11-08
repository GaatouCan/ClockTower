#include "GameWorld.h"

#include "utils.h"
#include "impl/PackageCodecImpl.h"
// #include "impl/ConnectionHandlerImpl.h"

#include "system/config/ConfigSystem.h"
#include "system/protocol/ProtocolSystem.h"
#include "system/manager/ManagerSystem.h"
#include "system/database/DatabaseSystem.h"
#include "system/login/LoginSystem.h"
#include "system/event/EventSystem.h"

#include <spdlog/spdlog.h>


UGameWorld::UGameWorld()
    : mAcceptor(mContext),
      mFullTimer(mContext),
      mConnectionFilter(nullptr),
      bInited(false),
      bRunning(false) {

    // Create Necessary Sub System
    CreateSystem<UConfigSystem>(0);
    CreateSystem<UProtocolSystem>(1);
    CreateSystem<ULoginSystem>(2);
    CreateSystem<UDatabaseSystem>(9);
    CreateSystem<UManagerSystem>(10);
    CreateSystem<UEventSystem>(11);
}

UGameWorld::~UGameWorld() {
    Shutdown();

    while (!mDestPriority.empty()) {
        auto [priority, type] = mDestPriority.top();
        mDestPriority.pop();

        const auto iter = mSystemMap.find(type);
        if (iter == mSystemMap.end())
            continue;

        delete iter->second;
        spdlog::info("{} destroyed.", type.name());

        mSystemMap.erase(iter);
    }

    // 正常情况下map应该是空了 但以防万一还是再遍历一次
    for (const auto sys: std::views::values(mSystemMap)) {
        delete sys;
    }

    spdlog::info("Game world destroyed.");
}

UGameWorld &UGameWorld::Init() {
    while (!mInitPriority.empty()) {
        auto [priority, type] = mInitPriority.top();
        mInitPriority.pop();

        const auto iter = mSystemMap.find(type);
        if (iter == mSystemMap.end())
            continue;

        iter->second->Init();
        spdlog::info("{} initialized.", iter->second->GetSystemName());
    }

    const auto &config = GetServerConfig();

    // Set PackagePool static option
    UPackagePool::LoadConfig(config);

    mPool.Start(config["server"]["work_thread"].as<size_t>());

    mMainThreadId = std::this_thread::get_id();
    bInited = true;

    return *this;
}

UGameWorld &UGameWorld::Run() {
    bRunning = true;

    asio::signal_set signals(mContext, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) {
        Shutdown();
    });

    co_spawn(mContext, WaitForConnect(), detached);
    mContext.run();

    return *this;
}

UGameWorld &UGameWorld::Shutdown() {
    if (!bInited)
        return *this;

    if (!bRunning)
        return *this;

    spdlog::info("Server shutting down...");
    bRunning = false;

    if (!mContext.stopped())
        mContext.stop();

    mConnectionMap.clear();

    return *this;
}

void UGameWorld::FilterConnection(const std::function<void(const AConnectionPointer&)> &filter) {
    mConnectionFilter = filter;
}

awaitable<void> UGameWorld::WaitForConnect() {
    const auto &config = GetServerConfig();

    try {
        mAcceptor.open(tcp::v4());
        mAcceptor.bind({tcp::v4(), config["server"]["port"].as<uint16_t>()});
        mAcceptor.listen();

        const auto loginSys = GetSystem<ULoginSystem>();
        if (loginSys == nullptr) {
            throw std::runtime_error("Failed to get login system");
        }

        spdlog::info("Waiting for client to connect - server port: {}", config["server"]["port"].as<uint16_t>());

        while (bRunning) {
            // PackagePool and io_context in per sub thread
            auto &[pool, ctx, tid] = mPool.NextContextNode();

            if (auto socket = co_await mAcceptor.async_accept(ctx); socket.is_open()) {
                const auto addr = socket.remote_endpoint().address();
                spdlog::info("New connection from: {}", addr.to_string());

                if (!loginSys->VerifyAddress(socket.remote_endpoint().address())) {
                    spdlog::warn("Rejected connection from: {}", addr.to_string());
                    continue;
                }

                const auto conn = std::make_shared<UConnection>(std::move(socket), pool);
                spdlog::info("Accept connection from: {}", addr.to_string());

                const std::string key = fmt::format("{} - {}", addr.to_string(), UnixTime());

                conn->SetThreadID(tid).SetKey(key);

                if (mConnectionFilter)
                    std::invoke(mConnectionFilter, conn);

                if (!conn->HasCodecSet())
                    conn->SetCodec<UPackageCodecImpl>();

                // if (!conn->HasHandlerSet())
                //     conn->SetHandler<UConnectionHandlerImpl>();

                conn->ConnectToClient();

                mConnectionMap[key] = conn;

                if (mConnectionMap.size() >= 1'000'000'000) {
                    mFullTimer.expires_after(10s);
                    co_await mFullTimer.async_wait();
                }
            }
        }
    } catch (std::exception &e) {
        spdlog::error("{} - {}", __func__, e.what());
        Shutdown();
    }
}

void UGameWorld::RemoveConnection(const std::string &key) {
    if (std::this_thread::get_id() != mMainThreadId) {
        co_spawn(mContext, [this, key]() mutable -> awaitable<void> {
            mConnectionMap.erase(key);
            co_return;
        }, detached);
        return;
    }
    mConnectionMap.erase(key);
}

FContextNode &UGameWorld::NextContextNode() {
    return mPool.NextContextNode();
}

asio::io_context &UGameWorld::GetIOContext() {
    return mContext;
}

AThreadID UGameWorld::GetThreadID() const {
    return mMainThreadId;
}
