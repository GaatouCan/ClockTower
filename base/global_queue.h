#pragma once

#include <queue>
#include <set>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>


class UReactor;
class UTaskQueue;


class UGlobalQueue final {

    std::queue<UTaskQueue *> mQueue;
    std::set<UTaskQueue *> mSet;
    std::map<UReactor *, UTaskQueue *> mReactorToQueue;
    std::set<UTaskQueue *> mEmptySet;

    std::mutex mMutex;
    mutable std::shared_mutex mSharedMutex;

    std::mutex mBlocking;
    std::condition_variable mCondVar;

    std::atomic_bool bQuit;

public:
    UGlobalQueue();
    ~UGlobalQueue();

    void Quit();

    UTaskQueue *RegisterReactor(UReactor *reactor);

    UTaskQueue *FindByReactor(UReactor *reactor) const;

    void OnPushTask(UTaskQueue *queue);

    UTaskQueue *WaitForQueue();
    void PushQueue(UTaskQueue *queue);
};
