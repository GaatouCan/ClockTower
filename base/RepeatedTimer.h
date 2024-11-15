#pragma once

#include "common.h"

using ATimerID = uint64_t;
using ATimerFunctor = std::function<void()>;

class URepeatedTimer final {

    AIOContext &mContext;
    ASteadyTimer mTimer;

    ATimerID mTimerId;
    ATimerFunctor mFunctor;

    std::chrono::duration<uint32_t> mExpire;
    bool bLoop;

    std::atomic<bool> bRunning;

public:
    URepeatedTimer() = delete;

    explicit URepeatedTimer(AIOContext &ctx);
    ~URepeatedTimer();

    DISABLE_COPY_MOVE(URepeatedTimer)

    URepeatedTimer &SetTimerID(ATimerID id);
    [[nodiscard]] ATimerID GetTimerID() const;

    URepeatedTimer &SetExpireTime(std::chrono::duration<uint32_t> expire);
    URepeatedTimer &SetLoop(bool loop);

    [[nodiscard]] bool IsLoop() const;
    [[nodiscard]] bool IsRunning() const;

    [[nodiscard]] bool IsLooping() const;

    template<typename Functor, typename... Args>
    URepeatedTimer &SetCallback(Functor &&func, Args &&... args) {
        mFunctor = [func = std::forward<Functor>(func), ...args = std::forward<Args>(args)] {
            std::invoke(func, args...);
        };
        return *this;
    }

    URepeatedTimer &Start();
    URepeatedTimer &Stop();
};
