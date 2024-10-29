#pragma once

#include "IPackageCodec.h"
#include "IConnectionHandler.h"
#include "TSDeque.h"
#include "PackagePool.h"

#include <memory>
#include <asio/experimental/awaitable_operators.hpp>

namespace base {

    using namespace std::literals::chrono_literals;
    using namespace asio::experimental::awaitable_operators;

    constexpr static auto CONNECTION_EXPIRE_SECONDS = 30s;
    constexpr static auto CONNECTION_WRITE_TIMEOUT = 10s;
    constexpr static auto CONNECTION_READ_TIMEOUT = 10s;

    constexpr static int NULL_CONTEXT_MAX_COUNT = 3;

    class UConnection final : public std::enable_shared_from_this<UConnection> {

        ATcpSocket socket_;
        UPackagePool &pool_;

        TSDeque<IPackage *> output_;

        std::unique_ptr<IPackageCodec> codec_ = nullptr;
        std::unique_ptr<IConnectionHandler> handler_ = nullptr;

        std::string key_;

        ASteadyTimer watchdogTimer_;
        ATimePoint deadline_;

        AThreadID tid_;

        uint32_t ctxNullCount_ = 0;
        std::any ctx_;

        static std::chrono::duration<uint32_t> expireTime;
        static std::chrono::duration<uint32_t> writeTimeout;
        static std::chrono::duration<uint32_t> readTimeout;

    public:
        UConnection() = delete;

        UConnection(ATcpSocket socket, UPackagePool &pool);
        ~UConnection();

        void ConnectToClient();
        void Disconnect();

        UConnection &SetContext(const std::any &ctx);
        UConnection &ResetContext();

        UConnection &SetKey(const std::string &key);

        static void SetWatchdogTimeout(uint32_t sec);
        static void SetWriteTimeout(uint32_t sec);
        static void SetReadTimeout(uint32_t sec);

        UConnection &SetThreadID(AThreadID tid);
        [[nodiscard]] AThreadID GetThreadID() const;
        [[nodiscard]] bool IsSameThread() const;

        template<typename T>
        requires std::derived_from<T, IPackageCodec>
        UConnection &SetCodec() {
            if (codec_ != nullptr)
                codec_.reset();

            codec_ = std::make_unique<T>();
            return *this;
        }

        template<typename T>
        requires std::derived_from<T, IConnectionHandler>
        UConnection &SetHandler() {
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

        [[nodiscard]] ATcpSocket &GetSocket() { return socket_; }

        [[nodiscard]] asio::ip::address RemoteAddress() const;

    private:
        awaitable<void> Watchdog();

        awaitable<void> WritePackage();
        awaitable<void> ReadPackage();

        static awaitable<void> Timeout(std::chrono::duration<uint32_t> expire);
    };
} // base
