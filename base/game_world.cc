#include "game_world.h"

#include "utils.h"
#include "impl/package_codec_impl.h"

#include "system/config/ConfigSystem.h"
#include "system/protocol/ProtocolSystem.h"
#include "system/login/LoginSystem.h"
#include "system/scene/SceneSystem.h"
#include "system/manager/ManagerSystem.h"
#include "system/event/EventSystem.h"


UGameWorld::UGameWorld()
    : mAcceptor(mContext),
      mFullTimer(mContext),
      mConnectionFilter(nullptr),
      bInited(false),
      bRunning(false) {
    // Create Sub System
    CreateSystem<UConfigSystem>(0);
    CreateSystem<UProtocolSystem>(1);
    CreateSystem<ULoginSystem>(2);
    CreateSystem<USceneSystem>(8);
    CreateSystem<UManagerSystem>(9);
    CreateSystem<UEventSystem>(10);
}

UGameWorld::~UGameWorld() {
    Shutdown();

    while (!mDestPriority.empty()) {
        auto [priority, type] = mDestPriority.top();
        mDestPriority.pop();

        const auto iter = mSystemMap.find(type);
        if (iter == mSystemMap.end())
            continue;

        spdlog::info("{} Destroyed.", iter->second->GetSystemName());
        delete iter->second;

        mSystemMap.erase(iter);
    }

    // 正常情况下map应该是空了 但以防万一还是再遍历一次
    for (const auto sys: std::views::values(mSystemMap)) {
        delete sys;
    }

    spdlog::info("Game World Destroyed.");
    spdlog::drop_all();
}

UGameWorld &UGameWorld::Init() {
    while (!mInitPriority.empty()) {
        auto [priority, type] = mInitPriority.top();
        mInitPriority.pop();

        const auto iter = mSystemMap.find(type);
        if (iter == mSystemMap.end())
            continue;

        iter->second->Init();
        spdlog::info("{} Initialized.", iter->second->GetSystemName());
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

    spdlog::info("Server Shutting Down...");
    bRunning = false;

    if (!mContext.stopped())
        mContext.stop();

    mConnectionMap.clear();

    return *this;
}

void UGameWorld::FilterConnection(const std::function<void(const AConnectionPointer &)> &filter) {
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
            spdlog::critical("{} - Failed to get login system.", __FUNCTION__);
            Shutdown();
            exit(-1);
        }

        spdlog::info("Waiting For Client To Connect - Server Port: {}", config["server"]["port"].as<uint16_t>());

        while (bRunning) {
            // PackagePool and io_context in per sub thread
            auto &[pool, ctx, tid, index] = mPool.NextContextNode();

            if (auto socket = co_await mAcceptor.async_accept(ctx); socket.is_open()) {
                const auto addr = socket.remote_endpoint().address();
                spdlog::info("New Connection From: {}", addr.to_string());

                if (!loginSys->VerifyAddress(socket.remote_endpoint().address())) {
                    socket.close();
                    spdlog::warn("Rejected Connection From: {}", addr.to_string());
                    continue;
                }

                std::string key;
                int count = 0;

                do {
                    key = fmt::format("{}-{}-{}", addr.to_string(), UnixTime(), absl::Uniform(mBitGen, 100, 999));
                    count++;
                } while (mConnectionMap.contains(key) && count < 3);

                if (count >= 3) {
                    socket.close();
                    spdlog::warn("Fail To Distribute Connection Key: {}", addr.to_string());
                    continue;
                }

                const auto conn = std::make_shared<UConnection>(std::move(socket), pool);
                spdlog::info("Accept Connection From: {}", addr.to_string());

                conn->SetThreadID(tid).SetNodeIndex(index).SetKey(key);

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
        spdlog::error("{} - {}", __FUNCTION__, e.what());
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

AIOContext &UGameWorld::GetIOContext() {
    return mContext;
}

AThreadID UGameWorld::GetThreadID() const {
    return mMainThreadId;
}
