﻿#include "connection.h"
#include "utils.h"

#include <spdlog/spdlog.h>


std::chrono::duration<uint32_t> UConnection::sExpireTime = 30s;
std::chrono::duration<uint32_t> UConnection::sWriteTimeout = 10s;
std::chrono::duration<uint32_t> UConnection::sReadTimeout = 10s;

UConnection::UConnection(ATcpSocket socket, UPackagePool &pool)
    : mSocket(std::move(socket)),
      mPool(pool),
      mNodeIndex(0),
      mWatchdogTimer(mSocket.get_executor()) {
}

UConnection::~UConnection() {
    Disconnect();
    spdlog::trace("UConnection::~UConnection() - {}", mKey);
}

void UConnection::ConnectToClient() {
    mDeadline = NowTimePoint() + sExpireTime;

    spdlog::debug("{} - Connection from {} run in thread: {}", __FUNCTION__, RemoteAddress().to_string(), ThreadIdToInt(mThreadId));
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

UConnection & UConnection::SetNodeIndex(const size_t idx) {
    mNodeIndex = idx;
    return *this;
}

size_t UConnection::GetNodeIndex() const {
    return mNodeIndex;
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
            now = NowTimePoint();

            if (mContextNullCount != -1) {
                if (!mContext.has_value()) {
                    mContextNullCount++;
                } else {
                    mContextNullCount = -1;
                }
            }
        } while (mDeadline > now && mContextNullCount < kNullContextMaxCount);

        if (mSocket.is_open()) {
            spdlog::warn("{} - Watchdog Timer Timeout - key[{}]", __FUNCTION__, mKey.empty() ? "null" : mKey);
            Disconnect();
        }
    } catch (std::exception &e) {
        spdlog::warn("{} - {} - key[{}]", __FUNCTION__, e.what(), mKey.empty() ? "null" : mKey);
    }
}

awaitable<void> UConnection::WritePackage() {
    try {
        if (mCodec == nullptr) {
            spdlog::critical("{} - codec undefined - key[{}]", __FUNCTION__, mKey.empty() ? "null" : mKey);
            Disconnect();
            co_return;
        }

        while (mSocket.is_open() && !mOutput.IsEmpty()) {
            const auto pkg = mOutput.PopFront();

            co_await mCodec->Encode(pkg);

            if (pkg->IsAvailable()) {
                if (mHandler != nullptr) {
                    co_await mHandler->OnWritePackage(pkg);
                }
                mPool.Recycle(pkg);
            } else {
                spdlog::warn("{} - Write Failed - key[{}]", __FUNCTION__, mKey.empty() ? "null" : mKey);
                mPool.Recycle(pkg);
                Disconnect();
            }
        }
    } catch (std::exception &e) {
        spdlog::error("{} - {} - key[{}]", __FUNCTION__, e.what(), mKey.empty() ? "null" : mKey);
        Disconnect();
    }
}

awaitable<void> UConnection::ReadPackage() {
    try {
        if (mCodec == nullptr) {
            spdlog::error("{} - PackageCodec Undefined - key[{}]", __FUNCTION__, mKey.empty() ? "null" : mKey);
            Disconnect();
            co_return;
        }

        while (mSocket.is_open()) {
            const auto pkg = BuildPackage();

            co_await mCodec->Decode(pkg);

            if (pkg->IsAvailable()) {
                mDeadline = NowTimePoint() + sExpireTime;

                if (mHandler != nullptr) {
                    co_await mHandler->OnReadPackage(pkg);
                }
            } else {
                spdlog::warn("{} - Read failed - key[{}]", __FUNCTION__, mKey.empty() ? "null" : mKey);
                Disconnect();
            }

            mPool.Recycle(pkg);
        }
    } catch (std::exception &e) {
        spdlog::error("{} - {} - key[{}]", __FUNCTION__, e.what(), mKey.empty() ? "null" : mKey);
        Disconnect();
    }
}

awaitable<void> UConnection::Timeout(const std::chrono::duration<uint32_t> expire) {
    ASystemTimer timer(co_await asio::this_coro::executor);
    timer.expires_after(expire);
    co_await timer.async_wait();
}
