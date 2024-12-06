#include "Manager.h"


IManager::IManager(FContextNode &ctx)
    : mContextNode(ctx) {
}


void IManager::CleanAllTimer() {
    std::scoped_lock lock(mTimerMutex);
    mTimerMap.clear();
}

AThreadID IManager::GetThreadID() const {
    return mContextNode.tid;
}

bool IManager::IsSameThread() const {
    return mContextNode.tid == std::this_thread::get_id();
}

void IManager::OnTick(ATimePoint now) {
}

AIOContext &IManager::GetIOContext() const {
    return mContextNode.ctx;
}

IPackage * IManager::BuildPackage() const {
    return mContextNode.pool.Acquire();
}

void IManager::StopTimer(const FUniqueID timerID) {
    std::scoped_lock lock(mTimerMutex);
    if (const auto iter = mTimerMap.find(timerID); iter != mTimerMap.end()) {
        iter->second.Stop();
        mTimerMap.erase(iter);
    }
}
