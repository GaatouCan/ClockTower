#include "global_queue.h"

#include "task_queue.h"
#include "reactor.h"

UGlobalQueue::UGlobalQueue() {
}

UGlobalQueue::~UGlobalQueue() {
}

UTaskQueue * UGlobalQueue::RegisterReactor(UReactor *reactor) {
    if (reactor == nullptr) {
        return nullptr;
    }

    if (const auto res = FindByReactor(reactor); res != nullptr) {
        return res;
    }

    auto *queue = new UTaskQueue(this);
    reactor->SetTaskQueue(queue);

    std::scoped_lock lock(mBlockMutex);

    mReactorToQueue[reactor] = queue;
    mQueueToReactor[queue] = reactor;

    mEmptySet.emplace(queue);

    return queue;
}

UTaskQueue * UGlobalQueue::FindByReactor(UReactor *reactor) const {
    std::shared_lock lock(mSharedMutex);
    if (const auto it = mReactorToQueue.find(reactor); it != mReactorToQueue.end()) {
        return it->second;
    }
    return nullptr;
}

UReactor * UGlobalQueue::FindByQueue(UTaskQueue *queue) const {
    std::shared_lock lock(mSharedMutex);
    if (const auto it = mQueueToReactor.find(queue); it != mQueueToReactor.end()) {
        return it->second;
    }
    return nullptr;
}
