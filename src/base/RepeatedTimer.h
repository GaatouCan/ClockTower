#pragma once

#include "../common/common.h"

namespace base {

    using TimerID = uint64_t;
    using TimerFunctor = std::function<void()>;

    class URepeatedTimer final {

        asio::io_context& ctx_;
        SteadyTimer timer_;

        TimerID id_;
        TimerFunctor func_;

        std::chrono::duration<uint32_t> expire_;
        bool loop_;

        std::atomic<bool> running_;

    public:
        URepeatedTimer() = delete;

        explicit URepeatedTimer(asio::io_context& ctx);
        ~URepeatedTimer();

        DISABLE_COPY_MOVE(URepeatedTimer)

        URepeatedTimer& SetTimerID(TimerID id);
        [[nodiscard]] TimerID GetTimerID() const;

        URepeatedTimer& SetExpireTime(std::chrono::duration<uint32_t> expire);
        URepeatedTimer& SetLoop(bool loop);

        template<typename FUNCTOR, typename... ARGS>
        URepeatedTimer &SetCallback(FUNCTOR &&func, ARGS&&... args) {
            func_ = [func = std::forward<FUNCTOR>(func), ...args = std::forward<ARGS>(args)] {
                std::invoke(func, args...);
            };
            return *this;
        }

        URepeatedTimer& Start();
        URepeatedTimer& Stop();

    };
} // base
