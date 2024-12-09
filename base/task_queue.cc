#include "task_queue.h"
#include "global_queue.h"
#include "reactor.h"

UTaskQueue::UTaskQueue(UGlobalQueue *global, UReactor *reactor)
    : mGlobal(global),
      mReactor(reactor),
      mCurrentQueue(new std::queue<AReactorTask>()),
      mWaitingQueue(new std::queue<AReactorTask>),
      bRunning(false),
      bInGlobal(false) {
    if (reactor != nullptr) {
        reactor->SetTaskQueue(this);
    }
}

UTaskQueue::~UTaskQueue() {
    delete mCurrentQueue;
    delete mWaitingQueue;
}

UReactor * UTaskQueue::GetReactor() const {
    return mReactor;
}

void UTaskQueue::PushTask(const AReactorTask &task) {
    if (bRunning) {
        std::scoped_lock lock(mMutex);
        mWaitingQueue->push(task);
    } else {
        std::scoped_lock lock(mMutex);
        mCurrentQueue->push(task);
        mGlobal->OnPushTask(this);
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

void UTaskQueue::OnRemoveFromGlobal() {
    bInGlobal = false;
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
