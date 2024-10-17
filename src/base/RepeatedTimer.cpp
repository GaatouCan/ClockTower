#include "RepeatedTimer.h"

#include <spdlog/spdlog.h>

namespace base {
    RepeatedTimer::RepeatedTimer(asio::io_context &ctx)
        : ctx_(ctx),
          timer_(ctx_),
          id_(0),
          expire_(std::chrono::seconds(0)),
          loop_(false),
          running_(false) {
    }

    RepeatedTimer::~RepeatedTimer() {
        Stop();
    }

    RepeatedTimer &RepeatedTimer::SetTimerID(const TimerID id) {
        id_ = id;
        return *this;
    }

    TimerID RepeatedTimer::GetTimerID() const {
        return id_;
    }

    RepeatedTimer &RepeatedTimer::SetExpireTime(const std::chrono::duration<uint32_t> expire) {
        expire_ = expire;
        return *this;
    }

    RepeatedTimer &RepeatedTimer::SetLoop(const bool loop) {
        loop_ = loop;
        return *this;
    }

    RepeatedTimer &RepeatedTimer::Start() {
        if (expire_.count() == 0)
            return *this;

        if (running_)
            return *this;

        if (!func_)
            return *this;

        co_spawn(ctx_, [this]() mutable -> awaitable<void> {
            try {
                running_ = true;
                TimePoint point = std::chrono::steady_clock::now();

                do {
                    point += expire_;
                    timer_.expires_at(point);
                    co_await timer_.async_wait();
                    if (running_)
                        std::invoke(func_);
                } while (running_ && loop_);

                running_ = false;
            } catch (std::exception &e) {
                spdlog::warn("RepeatedTimer::Start: {}", e.what());
            }
        }, detached);

        return *this;
    }

    RepeatedTimer & RepeatedTimer::Stop() {
        running_ = false;
        timer_.cancel();
        return *this;
    }
} // base
