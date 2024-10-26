#include "GameWorld.h"

#include "../common/utils.h"
#include "impl/PackageCodecImpl.h"
#include "impl/ConnectionHandlerImpl.h"

#include "../system/config/ConfigSystem.h"
#include "../system/protocol/ProtocolSystem.h"
#include "../system/manager/ManagerSystem.h"
#include "../system/database/DatabaseSystem.h"
#include "../system/login/LoginSystem.h"

#include <spdlog/spdlog.h>

namespace base {
    REGISTER_SYSTEM(ConfigSystem, 0)
    REGISTER_SYSTEM(ProtocolSystem, 1)
    REGISTER_SYSTEM(LoginSystem, 2)
    // REGISTER_SYSTEM(DatabaseSystem, 9)
    REGISTER_SYSTEM(ManagerSystem, 10)

    GameWorld::GameWorld()
        : acceptor_(ctx_),
          fullTimer_(ctx_),
          inited_(false),
          running_(false) {
    }

    GameWorld::~GameWorld() {
        Shutdown();

        while (!destPriority_.empty()) {
            auto [priority, type] = destPriority_.top();
            destPriority_.pop();

            const auto iter = systemMap_.find(type);
            if (iter == systemMap_.end())
                continue;

            delete iter->second;
            spdlog::info("{} destroyed", type.name());

            systemMap_.erase(iter);
        }

        // 正常情况下map应该是空了 但以防万一还是再遍历一次
        for (const auto sys: std::views::values(systemMap_)) {
            delete sys;
        }

        spdlog::info("Game world destroyed.");
    }

    GameWorld &GameWorld::Init() {
        while (!initPriority_.empty()) {
            auto [priority, type] = initPriority_.top();
            initPriority_.pop();

            const auto iter = systemMap_.find(type);
            if (iter == systemMap_.end())
                continue;

            iter->second->Init();
            spdlog::info("{} initialized.", iter->second->GetSystemName());
        }

        const auto &config = GetServerConfig();

        // Set PackagePool static option
        PackagePool::LoadConfig(config);

        pool_.Start(config["server"]["work_thread"].as<size_t>());

        tid_ = std::this_thread::get_id();
        inited_ = true;

        return *this;
    }

    GameWorld &GameWorld::Run() {
        running_ = true;

        asio::signal_set signals(ctx_, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            Shutdown();
        });

        co_spawn(ctx_, WaitForConnect(), detached);
        ctx_.run();

        return *this;
    }

    GameWorld &GameWorld::Shutdown() {
        if (!inited_)
            return *this;

        if (!running_)
            return *this;

        spdlog::info("Server shutting down...");
        running_ = false;

        if (!ctx_.stopped())
            ctx_.stop();

        connMap_.clear();

        return *this;
    }

    awaitable<void> GameWorld::WaitForConnect() {
        const auto &config = GetServerConfig();

        try {
            acceptor_.open(tcp::v4());
            acceptor_.bind({tcp::v4(), config["server"]["port"].as<uint16_t>()});
            acceptor_.listen();

            const auto loginSys = GetSystem<LoginSystem>();
            if (loginSys == nullptr) {
                throw std::runtime_error("Failed to get login system");
            }

            spdlog::info("Waiting for client to connect - server port: {}", config["server"]["port"].as<uint16_t>());

            while (running_) {
                // PackagePool and io_context in per sub thread
                auto &[pool, ctx, tid] = pool_.NextContextNode();

                if (auto [ec, socket] = co_await acceptor_.async_accept(ctx); socket.is_open()) {
                    const auto addr = socket.remote_endpoint().address();
                    spdlog::info("New connection from: {}", addr.to_string());

                    if (!loginSys->VerifyAddress(socket.remote_endpoint().address())) {
                        spdlog::warn("Rejected connection from: {}", addr.to_string());
                        continue;
                    }

                    const auto conn = std::make_shared<Connection>(std::move(socket), pool);
                    spdlog::info("Accept connection from: {}", addr.to_string());

                    const std::string key = fmt::format("{} - {}", addr.to_string(), CurrentTimeCount());

                    conn->SetCodec<PackageCodecImpl>()
                            .SetHandler<ConnectionHandlerImpl>()
                            .SetThreadID(tid)
                            .SetWatchdogTimeout(30)
                            .SetWriteTimeout(5)
                            .SetReadTimeout(5)
                            .SetKey(key)
                            .ConnectToClient();

                    connMap_[key] = conn;

                    if (connMap_.size() >= 1'000'000'000) {
                        fullTimer_.expires_after(10s);
                        co_await fullTimer_.async_wait();
                    }
                }
            }
        } catch (std::exception &e) {
            spdlog::error("{} - {}", __func__, e.what());
            Shutdown();
        }
    }

    void GameWorld::RemoveConnection(const std::string &key) {
        if (std::this_thread::get_id() != tid_) {
            co_spawn(ctx_, [this, key]() mutable -> awaitable<void> {
                connMap_.erase(key);
                co_return;
            }, detached);
            return;
        }
        connMap_.erase(key);
    }

    ContextNode &GameWorld::NextContextNode() {
        return pool_.NextContextNode();
    }

    asio::io_context &GameWorld::GetIOContext() {
        return ctx_;
    }

    ThreadID GameWorld::GetThreadID() const {
        return tid_;
    }
} // base
