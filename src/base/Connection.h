#pragma once

#include "IPackageCodec.h"
#include "IConnectionHandler.h"
#include "TSDeque.h"
#include "PackagePool.h"

#include <memory>
#include <asio/experimental/awaitable_operators.hpp>

namespace base {
    using ConnectionPtr = std::shared_ptr<class Connection>;

    using namespace std::literals::chrono_literals;
    using namespace asio::experimental::awaitable_operators;

    constexpr static int NULL_CONTEXT_MAX_COUNT = 3;

    class Connection final : public std::enable_shared_from_this<Connection> {

        TcpSocket socket_;
        PackagePool &pool_;

        TSDeque<IPackage *> output_;

        std::unique_ptr<IPackageCodec> codec_ = nullptr;
        std::unique_ptr<IConnectionHandler> handler_ = nullptr;

        std::string key_;

        SteadyTimer watchdogTimer_;
        TimePoint deadline_;

        std::chrono::duration<uint32_t> expireTime_ = 30s;
        std::chrono::duration<uint32_t> writeTimeout_ = 10s;
        std::chrono::duration<uint32_t> readTimeout_ = 10s;

        uint32_t ctxNullCount_ = 0;
        std::any ctx_;

    public:
        Connection() = delete;

        Connection(TcpSocket socket, PackagePool &pool);
        ~Connection();

        void connect();
        void disconnect();

        Connection &setContext(const std::any &ctx);
        Connection &resetContext();

        Connection &setKey(const std::string &key);

        Connection &setWatchdogTimeout(uint32_t sec);
        Connection &setWriteTimeout(uint32_t sec);
        Connection &setReadTimeout(uint32_t sec);

        template<typename T>
        requires std::derived_from<T, IPackageCodec>
        Connection &setCodec() {
            if (codec_ != nullptr)
                codec_.reset();

            codec_ = std::make_unique<T>();
            return *this;
        }

        template<typename T>
        requires std::derived_from<T, IConnectionHandler>
        Connection &setHandler() {
            if (handler_ != nullptr)
                handler_.reset();

            handler_ = std::make_unique<T>();
            return *this;
        }


        IPackage *buildPackage() const;
        void send(IPackage *pkg);

        [[nodiscard]] bool isConnected() const { return socket_.is_open(); }
        [[nodiscard]] std::string key() const { return key_; }
        [[nodiscard]] const std::any &context() const { return ctx_; }

        [[nodiscard]] TcpSocket &socket() { return socket_; }

        [[nodiscard]] asio::ip::address remoteAddress() const;

    private:
        awaitable<void> watchdog();

        awaitable<void> write();
        awaitable<void> read();

        static awaitable<void> timeout(std::chrono::duration<uint32_t> expire);
    };
} // base
