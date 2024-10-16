#include "Connection.h"

#include <spdlog/spdlog.h>

namespace base {
    Connection::Connection(TcpSocket socket, PackagePool &pool)
        : socket_(std::move(socket)),
          pool_(pool),
          watchdogTimer_(socket_.get_executor()) {
    }

    Connection::~Connection() {
        disconnect();
    }

    void Connection::connect() {
        deadline_ = std::chrono::steady_clock::now() + expireTime_;

        if (handler_ != nullptr)
            handler_->onConnected(shared_from_this());

        co_spawn(socket_.get_executor(), [self = shared_from_this()]() mutable -> awaitable<void> {
            try {
                co_await (self->read() || self->watchdog());
            } catch (std::exception &e) {
                spdlog::error("Connection::connect() - {}", e.what());
            }
        }, detached);
    }

    void Connection::disconnect() {
        if (socket_.is_open()) {
            socket_.close();
            if (handler_ != nullptr)
                handler_->onClosed(shared_from_this());
        }

        while (output_.empty()) {
            pool_.recycle(output_.popFront());
        }
    }

    Connection & Connection::setContext(const std::any &ctx) {
        ctx_ = ctx;
        return *this;
    }

    Connection & Connection::resetContext() {
        ctx_.reset();
        return *this;
    }

    Connection & Connection::setKey(const std::string &key) {
        key_ = key;
        return *this;
    }

    Connection & Connection::setWatchdogTimeout(uint32_t sec) {
        expireTime_ = std::chrono::seconds(sec);
        return *this;
    }

    Connection & Connection::setWriteTimeout(uint32_t sec) {
        writeTimeout_ = std::chrono::seconds(sec);
        return *this;
    }

    Connection & Connection::setReadTimeout(uint32_t sec) {
        readTimeout_ = std::chrono::seconds(sec);
        return *this;
    }

    IPackage * Connection::buildPackage() const {
        return pool_.acquire();
    }

    void Connection::send(IPackage *pkg) {
        const bool bEmpty = output_.empty();
        output_.pushBack(pkg);

        if (bEmpty)
            co_spawn(socket_.get_executor(), write(), detached);
    }

    asio::ip::address Connection::remoteAddress() const {
        if (isConnected())
            return socket_.remote_endpoint().address();
        return {};
    }

    awaitable<void> Connection::watchdog() {
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
                spdlog::error("Watchdog timer timeout {}", socket_.remote_endpoint().address().to_string());
                disconnect();
            }
        } catch (std::exception &e) {
            spdlog::error("{} : {}", __func__, e.what());
        }
    }

    awaitable<void> Connection::write() {
        try {
            if (codec_ == nullptr) {
                spdlog::error("{} - codec undefined", __func__);
                disconnect();
                co_return;
            }

            while (socket_.is_open() && !output_.empty()) {
                const auto pkg = output_.popFront();

                co_await codec_->encode(socket_, pkg);

                if (pkg->isAvailable()) {
                    if (handler_ != nullptr) {
                        co_await handler_->onWritePackage(shared_from_this());
                    }
                } else
                    spdlog::error("{} - write failed", __func__);

                pool_.recycle(pkg);
            }
        } catch (std::exception &e) {
            spdlog::error("{} : {}", __func__, e.what());
            disconnect();
        }
    }

    awaitable<void> Connection::read() {
        try {
            if (codec_ == nullptr) {
                spdlog::error("{} - codec undefined", __func__);
                disconnect();
                co_return;
            }

            while (socket_.is_open()) {
                const auto pkg = buildPackage();

                co_await codec_->decode(socket_, pkg);

                if (!pkg->isAvailable()) {
                    deadline_ = std::chrono::steady_clock::now() + expireTime_;

                    if (handler_ != nullptr) {
                        co_await handler_->onReadPackage(shared_from_this(), pkg);
                    }
                } else
                    spdlog::error("{} - read failed", __func__);

                pool_.recycle(pkg);
            }
        } catch (std::exception &e) {
            spdlog::error("{} : {}", __func__, e.what());
            disconnect();
        }
    }

    awaitable<void> Connection::timeout(const std::chrono::duration<uint32_t> expire) {
        SteadyTimer timer(co_await asio::this_coro::executor);
        timer.expires_after(expire);
        co_await timer.async_wait();
    }
} // base
