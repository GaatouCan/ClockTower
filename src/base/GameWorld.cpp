#include "GameWorld.h"


#include "../common/utils.h"
#include "impl/PackageCodecImpl.h"
#include "impl/ConnectionHandlerImpl.h"

#include <spdlog/spdlog.h>

namespace base {
    GameWorld::GameWorld()
        : acceptor_(ctx_),
          fullTimer_(ctx_),
          inited_(false),
          running_(false) {
    }

    GameWorld::~GameWorld() {
        shutdown();
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

            while (running_) {
                // PackagePool and io_context in per sub thread
                auto &[pool, ctx] = pool_.nextContextNode();

                if (auto [ec, socket] = co_await acceptor_.async_accept(ctx); socket.is_open()) {
                    const std::string addr = socket.remote_endpoint().address().to_string();
                    spdlog::debug("new connection from: {}", addr);

                    const auto conn = std::make_shared<Connection>(std::move(socket), pool);

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