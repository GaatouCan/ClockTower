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
    REGISTER_SYSTEM(DatabaseSystem, 9)
    REGISTER_SYSTEM(ManagerSystem, 10)

    GameWorld::GameWorld()
        : acceptor_(ctx_),
          fullTimer_(ctx_),
          inited_(false),
          running_(false) {

    }

    GameWorld::~GameWorld() {
        shutdown();

        for (const auto sys : std::views::values(systemMap_)) {
            delete sys;
        }
    }

    GameWorld & GameWorld::init() {
        while (!initPriority_.empty()) {
            auto [priority, type] = initPriority_.top();
            initPriority_.pop();

            const auto iter = systemMap_.find(type);
            if (iter == systemMap_.end())
                continue;

            iter->second->init();
            spdlog::info("{} initialized.", iter->second->name());
        }

        pool_.start(4);

        inited_ = true;
        return *this;
    }

    GameWorld & GameWorld::run() {
        running_ = true;

        asio::signal_set signals(ctx_, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            shutdown();
        });

        co_spawn(ctx_, waitForConnect(), detached);
        ctx_.run();

        return *this;
    }

    GameWorld & GameWorld::shutdown() {
        if (!inited_)
            return *this;

        if (!running_)
            return *this;

        running_ = false;

        if (!ctx_.stopped())
            ctx_.stop();

        connMap_.clear();

        return *this;
    }

    awaitable<void> GameWorld::waitForConnect() {
        try {
            acceptor_.open(tcp::v4());
            acceptor_.bind({tcp::v4(), 8080});

            const auto loginSys = GetSystem<LoginSystem>();
            if (loginSys == nullptr) {
                throw std::runtime_error("Failed to get login system");
            }

            while (running_) {
                // PackagePool and io_context in per sub thread
                auto &[pool, ctx] = pool_.nextContextNode();

                if (auto [ec, socket] = co_await acceptor_.async_accept(ctx); socket.is_open()) {
                    const std::string addr = socket.remote_endpoint().address().to_string();
                    spdlog::debug("New connection from: {}", addr);

                    if (!loginSys->verifyAddress(socket.remote_endpoint().address())) {
                        spdlog::debug("Rejected connection from: {}", addr);
                        continue;
                    }

                    const auto conn = std::make_shared<Connection>(std::move(socket), pool);
                    spdlog::debug("Accept connection from: {}", addr);

                    const std::string key = fmt::format("{} - {}", addr, CurrentTimeCount());

                    conn->setCodec<PackageCodecImpl>()
                            .setHandler<ConnectionHandlerImpl>()
                            .setWatchdogTimeout(30)
                            .setWriteTimeout(5)
                            .setReadTimeout(5)
                            .setKey(key)
                            .connect();

                    connMap_[key] = conn;

                    if (connMap_.size() >= 1'000'000'000) {
                        fullTimer_.expires_after(10s);
                        co_await fullTimer_.async_wait();
                    }
                }
            }
        } catch (std::exception &e) {
            spdlog::error("{} - {}", __func__, e.what());
        }
    }

    void GameWorld::removeConnection(const std::string &key) {
        if (std::this_thread::get_id() != threadId_) {
            co_spawn(ctx_, [this, &key]() mutable -> awaitable<void> {
                connMap_.erase(key);
                co_return;
            }, detached);
            return;
        }
        connMap_.erase(key);
    }
} // base
