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

        void ConnectToClient();
        void Disconnect();

        Connection &SetContext(const std::any &ctx);
        Connection &ResetContext();

        Connection &SetKey(const std::string &key);

        Connection &SetWatchdogTimeout(uint32_t sec);
        Connection &SetWriteTimeout(uint32_t sec);
        Connection &SetReadTimeout(uint32_t sec);

        template<typename T>
        requires std::derived_from<T, IPackageCodec>
        Connection &SetCodec() {
            if (codec_ != nullptr)
                codec_.reset();

            codec_ = std::make_unique<T>();
            return *this;
        }

        template<typename T>
        requires std::derived_from<T, IConnectionHandler>
        Connection &SetHandler() {
            if (handler_ != nullptr)
                handler_.reset();

            handler_ = std::make_unique<T>();
            return *this;
        }

        IPackage *BuildPackage() const;
        void Send(IPackage *pkg);

        [[nodiscard]] bool IsConnected() const { return socket_.is_open(); }
        [[nodiscard]] std::string GetKey() const { return key_; }
        [[nodiscard]] const std::any &GetContext() const { return ctx_; }

        [[nodiscard]] TcpSocket &GetSocket() { return socket_; }

        [[nodiscard]] asio::ip::address RemoteAddress() const;

    private:
        awaitable<void> Watchdog();

        awaitable<void> WritePackage();
        awaitable<void> ReadPackage();

        static awaitable<void> Timeout(std::chrono::duration<uint32_t> expire);
    };
} // base
