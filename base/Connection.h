#pragma once

#include "PackageCodec.h"
#include "ConnectionHandler.h"
#include "TSDeque.h"
#include "PackagePool.h"

#include <memory>
#include <asio/experimental/awaitable_operators.hpp>

using namespace std::literals::chrono_literals;
using namespace asio::experimental::awaitable_operators;

class UConnection final : public std::enable_shared_from_this<UConnection> {

    ATcpSocket mSocket;
    UPackagePool &mPool;

    TSDeque<IPackage *> mOutput;

    std::unique_ptr<IPackageCodec> mCodec = nullptr;
    std::unique_ptr<IConnectionHandler> mHandler = nullptr;

    std::string mKey;

    ASteadyTimer mWatchdogTimer;
    ATimePoint mDeadline;

    AThreadID mThreadId;

    uint32_t mContextNullCount = 0;
    std::any mContext;

    static std::chrono::duration<uint32_t> sExpireTime;
    static std::chrono::duration<uint32_t> sWriteTimeout;
    static std::chrono::duration<uint32_t> sReadTimeout;

    static constexpr int kNullContextMaxCount = 3;

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

    [[nodiscard]] bool HasCodecSet() const;

    template<typename T>
    requires std::derived_from<T, IPackageCodec>
    UConnection &SetCodec() {
        if (mCodec != nullptr)
            mCodec.reset();

        mCodec = std::make_unique<T>();
        return *this;
    }

    [[nodiscard]] bool HasHandlerSet() const;

    template<typename T>
    requires std::derived_from<T, IConnectionHandler>
    UConnection &SetHandler() {
        if (mHandler != nullptr)
            mHandler.reset();

        mHandler = std::make_unique<T>();
        return *this;
    }

    IPackage *BuildPackage() const;

    void Send(IPackage *pkg);

    [[nodiscard]] bool IsConnected() const { return mSocket.is_open(); }
    [[nodiscard]] std::string GetKey() const { return mKey; }
    [[nodiscard]] const std::any &GetContext() const { return mContext; }

    [[nodiscard]] ATcpSocket &GetSocket() { return mSocket; }

    [[nodiscard]] asio::ip::address RemoteAddress() const;

private:
    awaitable<void> Watchdog();
    awaitable<void> WritePackage();
    awaitable<void> ReadPackage();

    static awaitable<void> Timeout(std::chrono::duration<uint32_t> expire);
};
