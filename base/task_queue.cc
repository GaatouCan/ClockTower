#include "task_queue.h"
#include "global_queue.h"

UTaskQueue::UTaskQueue(UGlobalQueue *global)
    : mGlobal(global),
      mCurrentQueue(new std::queue<AReactorTask>()),
      mWaitingQueue(new std::queue<AReactorTask>),
      bRunning(false),
      bInGlobal(false) {
}

UTaskQueue::~UTaskQueue() {
    delete mCurrentQueue;
    delete mWaitingQueue;
}

void UTaskQueue::PushTask(const AReactorTask &task) {
    std::scoped_lock lock(mMutex);
    if (bRunning) {
        mWaitingQueue->push(task);
    } else {
        mCurrentQueue->push(task);
    }
}

bool UTaskQueue::IsEmpty() const {
    std::shared_lock lock(mSharedMutex);
    return bRunning ? mWaitingQueue->empty() : mCurrentQueue->empty();
}

bool UTaskQueue::IsRunning() const {
    return bRunning;
}

void UTaskQueue::OnAddToGlobal() {
    bInGlobal = true;
}

bool UTaskQueue::IsInGlobal() const {
    return bInGlobal;
}

void UTaskQueue::OnStart() {
    bRunning = true;
}

void UTaskQueue::OnStop() {
    bRunning = false;

    std::scoped_lock lock(mMutex);
    while (!mWaitingQueue->empty()) {
        auto val = std::move(mWaitingQueue->front());
        mWaitingQueue->pop();
        mCurrentQueue->emplace(val);
    }
}
