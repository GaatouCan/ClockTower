#include "Connection.h"
#include "utils.h"

#include <spdlog/spdlog.h>


std::chrono::duration<uint32_t> UConnection::sExpireTime = 30s;
std::chrono::duration<uint32_t> UConnection::sWriteTimeout = 10s;
std::chrono::duration<uint32_t> UConnection::sReadTimeout = 10s;

UConnection::UConnection(ATcpSocket socket, UPackagePool &pool)
    : mSocket(std::move(socket)),
      mPool(pool),
      mWatchdogTimer(mSocket.get_executor()) {
}

UConnection::~UConnection() {
    Disconnect();
    spdlog::trace("UConnection::~UConnection() - {}", mKey);
}

void UConnection::ConnectToClient() {
    mDeadline = std::chrono::steady_clock::now() + sExpireTime;

    spdlog::trace("{} - Connection from {} run in thread: {}", __FUNCTION__, RemoteAddress().to_string(), ThreadIdToInt(mThreadId));
    if (mHandler != nullptr)
        mHandler->OnConnected();

    co_spawn(mSocket.get_executor(), [self = shared_from_this()]() mutable -> awaitable<void> {
        try {
            co_await (self->ReadPackage() || self->Watchdog());
        } catch (std::exception &e) {
            spdlog::warn("Connection::connect() - {}", e.what());
        }
    }, detached);
}

void UConnection::Disconnect() {
    if (mSocket.is_open()) {
        mSocket.close();
        if (mHandler != nullptr)
            mHandler->OnClosed();
    }

    while (!mOutput.IsEmpty()) {
        mPool.Recycle(mOutput.PopFront());
    }
}

UConnection &UConnection::SetContext(const std::any &ctx) {
    mContext = ctx;
    return *this;
}

UConnection &UConnection::ResetContext() {
    mContext.reset();
    return *this;
}

UConnection &UConnection::SetKey(const std::string &key) {
    mKey = key;
    return *this;
}

void UConnection::SetWatchdogTimeout(const uint32_t sec) {
    sExpireTime = std::chrono::seconds(sec);
}

void UConnection::SetWriteTimeout(const uint32_t sec) {
    sWriteTimeout = std::chrono::seconds(sec);
}

void UConnection::SetReadTimeout(const uint32_t sec) {
    sReadTimeout = std::chrono::seconds(sec);
}

UConnection &UConnection::SetThreadID(const AThreadID tid) {
    mThreadId = tid;
    return *this;
}

AThreadID UConnection::GetThreadID() const {
    return mThreadId;
}

bool UConnection::IsSameThread() const {
    return mThreadId == std::this_thread::get_id();
}

bool UConnection::HasCodecSet() const {
    return mCodec != nullptr;
}

bool UConnection::HasHandlerSet() const {
    return mHandler != nullptr;
}

IPackage *UConnection::BuildPackage() const {
    return mPool.Acquire();
}

void UConnection::Send(IPackage *pkg) {
    const bool bEmpty = mOutput.IsEmpty();
    mOutput.PushBack(pkg);

    if (bEmpty)
        co_spawn(mSocket.get_executor(), WritePackage(), detached);
}

asio::ip::address UConnection::RemoteAddress() const {
    if (IsConnected())
        return mSocket.remote_endpoint().address();
    return {};
}

awaitable<void> UConnection::Watchdog() {
    try {
        decltype(mDeadline) now;
        do {
            mWatchdogTimer.expires_at(mDeadline);
            co_await mWatchdogTimer.async_wait();
            now = std::chrono::steady_clock::now();

            if (mContextNullCount != -1) {
                if (!mContext.has_value()) {
                    mContextNullCount++;
                } else {
                    mContextNullCount = -1;
                }
            }
        } while (mDeadline > now && mContextNullCount < kNullContextMaxCount);

        if (mSocket.is_open()) {
            spdlog::warn("Watchdog Timer Timeout {}", mSocket.remote_endpoint().address().to_string());
            Disconnect();
        }
    } catch (std::exception &e) {
        spdlog::warn("{} - {}", __FUNCTION__, e.what());
    }
}

awaitable<void> UConnection::WritePackage() {
    try {
        if (mCodec == nullptr) {
            spdlog::critical("{} - codec undefined", __FUNCTION__);
            Disconnect();
            co_return;
        }

        while (mSocket.is_open() && !mOutput.IsEmpty()) {
            const auto pkg = mOutput.PopFront();

            co_await mCodec->Encode(pkg);

            if (pkg->IsAvailable()) {
                if (mHandler != nullptr) {
                    mHandler->OnWritePackage(pkg);
                }
                mPool.Recycle(pkg);
            } else {
                spdlog::warn("{} - write failed", __FUNCTION__);
                mPool.Recycle(pkg);
                Disconnect();
            }
        }
    } catch (std::exception &e) {
        spdlog::error("{} : {}", __FUNCTION__, e.what());
        Disconnect();
    }
}

awaitable<void> UConnection::ReadPackage() {
    try {
        if (mCodec == nullptr) {
            spdlog::error("{} - codec undefined", __FUNCTION__);
            Disconnect();
            co_return;
        }

        while (mSocket.is_open()) {
            const auto pkg = BuildPackage();

            co_await mCodec->Decode(pkg);

            if (pkg->IsAvailable()) {
                mDeadline = std::chrono::steady_clock::now() + sExpireTime;

                if (mHandler != nullptr) {
                    mHandler->OnReadPackage(pkg);
                }
            } else {
                spdlog::warn("{} - Read failed", __FUNCTION__);
                Disconnect();
            }

            mPool.Recycle(pkg);
        }
    } catch (std::exception &e) {
        spdlog::error("{} : {}", __FUNCTION__, e.what());
        Disconnect();
    }
}

awaitable<void> UConnection::Timeout(const std::chrono::duration<uint32_t> expire) {
    ASteadyTimer timer(co_await asio::this_coro::executor);
    timer.expires_after(expire);
    co_await timer.async_wait();
}
