#pragma once

#include "common.h"
#include "GeneratedID.h"

// using ATimerID = FGeneratedID;
using ATimerFunctor = std::function<void()>;

class URepeatedTimer final {

    AIOContext &mContext;
    ASystemTimer mTimer;

    FGeneratedID mTimerId;
    ATimerFunctor mFunctor;

    std::chrono::duration<uint32_t> mExpire;
    bool bLoop;

    std::atomic<bool> bRunning;

public:
    URepeatedTimer() = delete;

    explicit URepeatedTimer(AIOContext &ctx);
    ~URepeatedTimer();

    DISABLE_COPY_MOVE(URepeatedTimer)

    URepeatedTimer &SetTimerID(FGeneratedID id);
    [[nodiscard]] FGeneratedID GetTimerID() const;

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
