#include "RepeatedTimer.h"

#include <spdlog/spdlog.h>


URepeatedTimer::URepeatedTimer(AIOContext &ctx)
    : mContext(ctx),
      mTimer(mContext),
      mTimerId(0),
      mExpire(std::chrono::seconds(0)),
      bLoop(false),
      bRunning(false) {
}

URepeatedTimer::~URepeatedTimer() {
    Stop();
}

URepeatedTimer &URepeatedTimer::SetTimerID(const FUniqueID id) {
    mTimerId = id;
    return *this;
}

FUniqueID URepeatedTimer::GetTimerID() const {
    return mTimerId;
}

URepeatedTimer &URepeatedTimer::SetExpireTime(const std::chrono::duration<uint32_t> expire) {
    mExpire = expire;
    return *this;
}

URepeatedTimer &URepeatedTimer::SetLoop(const bool loop) {
    bLoop = loop;
    return *this;
}

bool URepeatedTimer::IsLoop() const {
    return bLoop;
}

bool URepeatedTimer::IsRunning() const {
    return bRunning;
}

bool URepeatedTimer::IsLooping() const {
    return bLoop && bRunning;
}

URepeatedTimer &URepeatedTimer::Start() {
    if (mExpire.count() == 0)
        return *this;

    if (bRunning)
        return *this;

    if (!mFunctor)
        return *this;

    co_spawn(mContext, [this]() mutable -> awaitable<void> {
        try {
            bRunning = true;
            auto point = NowTimePoint();

            do {
                point += mExpire;
                mTimer.expires_at(point);

                co_await mTimer.async_wait();

                if (bRunning)
                    std::invoke(mFunctor);
            } while (bRunning && bLoop);

            bRunning = false;
        } catch (std::exception &e) {
            spdlog::warn("RepeatedTimer::Start: {}", e.what());
        }
    }, detached);

    return *this;
}

URepeatedTimer &URepeatedTimer::Stop() {
    if (!bRunning)
        return *this;

    bRunning = false;
    mTimer.cancel();
    return *this;
}
