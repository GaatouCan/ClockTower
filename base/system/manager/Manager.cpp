#include "Manager.h"


IManager::IManager(FContextNode &ctx)
    : mContextNode(ctx) {
}


void IManager::CleanAllTimer() {
    std::scoped_lock lock(mTimerMutex);
    mTimerMap.clear();
}

AThreadID IManager::GetThreadID() const {
    return mContextNode.mThreadID;
}

bool IManager::IsSameThread() const {
    return mContextNode.mThreadID == std::this_thread::get_id();
}

void IManager::OnTick(ATimePoint now) {
}

void IManager::OnDayChange() {
}

AIOContext &IManager::GetIOContext() const {
    return mContextNode.mIOContext;
}

IPackage * IManager::BuildPackage() const {
    return mContextNode.mPool.Acquire();
}

void IManager::StopTimer(const FUniqueID timerID) {
    std::scoped_lock lock(mTimerMutex);
    if (const auto iter = mTimerMap.find(timerID); iter != mTimerMap.end()) {
        iter->second.Stop();
        mTimerMap.erase(iter);
    }
}
