#pragma once

#include "../common/common.h"

namespace base {

    using TimerID = uint64_t;
    using TimerFunctor = std::function<void()>;

    class RepeatedTimer final {

        asio::io_context& ctx_;
        SteadyTimer timer_;

        TimerID id_;
        TimerFunctor func_;

        std::chrono::duration<uint32_t> expire_;
        bool loop_;

        std::atomic<bool> running_;

    public:
        RepeatedTimer() = delete;

        explicit RepeatedTimer(asio::io_context& ctx);

        DISABLE_COPY_MOVE(RepeatedTimer)

        ~RepeatedTimer();

        RepeatedTimer& SetTimerID(TimerID id);
        [[nodiscard]] TimerID GetTimerID() const;

        RepeatedTimer& SetExpireTime(std::chrono::duration<uint32_t> expire);
        RepeatedTimer& SetLoop(bool loop);

        template<typename FUNCTOR, typename... ARGS>
        RepeatedTimer &SetCallback(FUNCTOR &&func, ARGS&&... args) {
            func_ = [func = std::forward<FUNCTOR>(func), ...args = std::forward<ARGS>(args)] {
                std::invoke(func, args...);
            };
            return *this;
        }

        RepeatedTimer& Start();
        RepeatedTimer& Stop();

    };
} // base
