#include "Manager.h"


IManager::IManager(asio::io_context &ctx)
    : mIOContext(ctx) {
}

void IManager::SetThreadID(const AThreadID tid) {
    mThreadId = tid;
}

AThreadID IManager::GetThreadID() const {
    return mThreadId;
}

bool IManager::IsSameThread() const {
    return mThreadId == std::this_thread::get_id();
}

void IManager::OnTick(ATimePoint now) {
}

asio::io_context &IManager::GetIOContext() const {
    return mIOContext;
}

void IManager::StopTimer(const ATimerID timerID) {
    if (const auto iter = mTimerMap.find(timerID); iter != mTimerMap.end()) {
        iter->second.Stop();
        mTimerMap.erase(iter);
    }
}