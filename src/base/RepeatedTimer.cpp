#include "RepeatedTimer.h"

#include <spdlog/spdlog.h>


URepeatedTimer::URepeatedTimer(asio::io_context &ctx)
    : ctx_(ctx),
      timer_(ctx_),
      id_(0),
      expire_(std::chrono::seconds(0)),
      loop_(false),
      running_(false) {
}

URepeatedTimer::~URepeatedTimer() {
    Stop();
}

URepeatedTimer &URepeatedTimer::SetTimerID(const ATimerID id) {
    id_ = id;
    return *this;
}

ATimerID URepeatedTimer::GetTimerID() const {
    return id_;
}

URepeatedTimer &URepeatedTimer::SetExpireTime(const std::chrono::duration<uint32_t> expire) {
    expire_ = expire;
    return *this;
}

URepeatedTimer &URepeatedTimer::SetLoop(const bool loop) {
    loop_ = loop;
    return *this;
}

URepeatedTimer &URepeatedTimer::Start() {
    if (expire_.count() == 0)
        return *this;

    if (running_)
        return *this;

    if (!func_)
        return *this;

    co_spawn(ctx_, [this]() mutable -> awaitable<void> {
        try {
            running_ = true;
            ATimePoint point = std::chrono::steady_clock::now();

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

URepeatedTimer &URepeatedTimer::Stop() {
    if (!running_)
        return *this;

    running_ = false;
    timer_.cancel();
    return *this;
}
