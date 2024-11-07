#include "Connection.h"
#include "../common/utils.h"

#include <spdlog/spdlog.h>


std::chrono::duration<uint32_t> UConnection::expireTime = 30s;
std::chrono::duration<uint32_t> UConnection::writeTimeout = 10s;
std::chrono::duration<uint32_t> UConnection::readTimeout = 10s;

UConnection::UConnection(ATcpSocket socket, UPackagePool &pool)
    : socket_(std::move(socket)),
      pool_(pool),
      watchdogTimer_(socket_.get_executor()) {
}

UConnection::~UConnection() {
    Disconnect();
}

void UConnection::ConnectToClient() {
    deadline_ = std::chrono::steady_clock::now() + expireTime;

    spdlog::trace("{} - Connection from {} run in thread: {}", __func__, RemoteAddress().to_string(),
                  ThreadIdToInt(tid_));
    if (handler_ != nullptr)
        handler_->OnConnected(shared_from_this());

    co_spawn(socket_.get_executor(), [self = shared_from_this()]() mutable -> awaitable<void> {
        try {
            co_await (self->ReadPackage() || self->Watchdog());
        } catch (std::exception &e) {
            spdlog::warn("Connection::connect() - {}", e.what());
        }
    }, detached);
}

void UConnection::Disconnect() {
    if (socket_.is_open()) {
        socket_.close();
        if (handler_ != nullptr)
            handler_->OnClosed(shared_from_this());
    }

    while (!output_.IsEmpty()) {
        pool_.Recycle(output_.PopFront());
    }
}

UConnection &UConnection::SetContext(const std::any &ctx) {
    ctx_ = ctx;
    return *this;
}

UConnection &UConnection::ResetContext() {
    ctx_.reset();
    return *this;
}

UConnection &UConnection::SetKey(const std::string &key) {
    key_ = key;
    return *this;
}

void UConnection::SetWatchdogTimeout(const uint32_t sec) {
    expireTime = std::chrono::seconds(sec);
}

void UConnection::SetWriteTimeout(const uint32_t sec) {
    writeTimeout = std::chrono::seconds(sec);
}

void UConnection::SetReadTimeout(const uint32_t sec) {
    readTimeout = std::chrono::seconds(sec);
}

UConnection &UConnection::SetThreadID(const AThreadID tid) {
    tid_ = tid;
    return *this;
}

AThreadID UConnection::GetThreadID() const {
    return tid_;
}

bool UConnection::IsSameThread() const {
    return tid_ == std::this_thread::get_id();
}

bool UConnection::HasCodecSet() const {
    return codec_ != nullptr;
}

bool UConnection::HasHandlerSet() const {
    return handler_ != nullptr;
}

IPackage *UConnection::BuildPackage() const {
    return pool_.Acquire();
}

void UConnection::Send(IPackage *pkg) {
    const bool bEmpty = output_.IsEmpty();
    output_.PushBack(pkg);

    if (bEmpty)
        co_spawn(socket_.get_executor(), WritePackage(), detached);
}

asio::ip::address UConnection::RemoteAddress() const {
    if (IsConnected())
        return socket_.remote_endpoint().address();
    return {};
}

awaitable<void> UConnection::Watchdog() {
    try {
        decltype(deadline_) now;
        do {
            watchdogTimer_.expires_at(deadline_);
            co_await watchdogTimer_.async_wait();
            now = std::chrono::steady_clock::now();

            if (ctxNullCount_ != -1) {
                if (!ctx_.has_value()) {
                    ctxNullCount_++;
                } else {
                    ctxNullCount_ = -1;
                }
            }
        } while (deadline_ > now && ctxNullCount_ < NULL_CONTEXT_MAX_COUNT);

        if (socket_.is_open()) {
            spdlog::warn("Watchdog Timer Timeout {}", socket_.remote_endpoint().address().to_string());
            Disconnect();
        }
    } catch (std::exception &e) {
        spdlog::error("{} - {}", __func__, e.what());
    }
}

awaitable<void> UConnection::WritePackage() {
    try {
        if (codec_ == nullptr) {
            spdlog::critical("{} - codec undefined", __func__);
            Disconnect();
            co_return;
        }

        while (socket_.is_open() && !output_.IsEmpty()) {
            const auto pkg = output_.PopFront();

            co_await codec_->Encode(socket_, pkg);

            if (pkg->IsAvailable()) {
                if (handler_ != nullptr) {
                    co_await handler_->OnWritePackage(shared_from_this());
                }
                pool_.Recycle(pkg);
            } else {
                spdlog::warn("{} - write failed", __func__);
                pool_.Recycle(pkg);
                Disconnect();
            }
        }
    } catch (std::exception &e) {
        spdlog::error("{} : {}", __func__, e.what());
        Disconnect();
    }
}

awaitable<void> UConnection::ReadPackage() {
    try {
        if (codec_ == nullptr) {
            spdlog::error("{} - codec undefined", __func__);
            Disconnect();
            co_return;
        }

        while (socket_.is_open()) {
            const auto pkg = BuildPackage();

            co_await codec_->Decode(socket_, pkg);

            if (pkg->IsAvailable()) {
                deadline_ = std::chrono::steady_clock::now() + expireTime;

                if (handler_ != nullptr) {
                    co_await handler_->OnReadPackage(shared_from_this(), pkg);
                }
            } else {
                spdlog::warn("{} - Read failed", __func__);
                Disconnect();
            }

            pool_.Recycle(pkg);
        }
    } catch (std::exception &e) {
        spdlog::error("{} : {}", __func__, e.what());
        Disconnect();
    }
}

awaitable<void> UConnection::Timeout(const std::chrono::duration<uint32_t> expire) {
    ASteadyTimer timer(co_await asio::this_coro::executor);
    timer.expires_after(expire);
    co_await timer.async_wait();
}
