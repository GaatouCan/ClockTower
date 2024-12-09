#include "global_queue.h"

#include "task_queue.h"
#include "reactor.h"

UGlobalQueue::UGlobalQueue()
    : bQuit(false) {
}

UGlobalQueue::~UGlobalQueue() {
    Quit();

    for (const auto it: mSet)
        delete it;
}

void UGlobalQueue::Quit() {
    bQuit = true;

    std::unique_lock lock(mBlocking);
    mCondVar.notify_all();
}

UTaskQueue *UGlobalQueue::RegisterReactor(UReactor *reactor) {
    if (reactor == nullptr) {
        return nullptr;
    }

    if (const auto res = FindByReactor(reactor); res != nullptr) {
        return res;
    }

    auto *queue = new UTaskQueue(this, reactor);

    std::scoped_lock lock(mMutex);

    assert(!mSet.contains(queue));

    mSet.emplace(queue);
    mReactorToQueue[reactor] = queue;
    mEmptySet.emplace(queue);

    return queue;
}

UTaskQueue *UGlobalQueue::FindByReactor(UReactor *reactor) const {
    if (reactor == nullptr)
        return nullptr;

    std::shared_lock lock(mSharedMutex);
    if (const auto it = mReactorToQueue.find(reactor); it != mReactorToQueue.end()) {
        return it->second;
    }
    return nullptr;
}

void UGlobalQueue::OnPushTask(UTaskQueue *queue) {
    if (queue == nullptr)
        return;

    if (queue->IsEmpty())
        return;

    {
        std::shared_lock lock(mSharedMutex);
        if (!mEmptySet.contains(queue))
            return;
    }

    {
        std::scoped_lock lock(mMutex);
        mEmptySet.erase(queue);
        mQueue.emplace(queue);
    }

    queue->OnAddToGlobal();

    std::unique_lock lock(mBlocking);
    mCondVar.notify_one();
}

UTaskQueue *UGlobalQueue::WaitForQueue() {
    {
        std::unique_lock lock(mBlocking);
        mCondVar.wait(lock, [this] { return !mQueue.empty() || bQuit; });
    }

    UTaskQueue *res = nullptr;

    {
        std::scoped_lock lock(mMutex);
        res = mQueue.front();
        mQueue.pop();
    }

    res->OnRemoveFromGlobal();
    return res;
}

void UGlobalQueue::PushQueue(UTaskQueue *queue) {
    if (queue == nullptr)
        return;

    if (!mSet.contains(queue))
        return;

    queue->OnStop();

    if (queue->IsEmpty()) {
        std::scoped_lock lock(mMutex);
        mEmptySet.emplace(queue);
        return;
    }

    {
        std::scoped_lock lock(mMutex);
        mQueue.emplace(queue);
    }

    queue->OnAddToGlobal();

    std::unique_lock lock(mBlocking);
    mCondVar.notify_one();
}
