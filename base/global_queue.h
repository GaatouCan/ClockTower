#pragma once

#include <queue>
#include <set>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

struct FQueueItem {
    class UReactor *mReactor;
    class UTaskQueue *mQueue;
};

class UGlobalQueue final {

    std::queue<FQueueItem> mQueue;

    std::map<UReactor *, UTaskQueue *> mReactorToQueue;
    std::map<UTaskQueue *, UReactor *> mQueueToReactor;

    std::set<UTaskQueue *> mEmptySet;

    std::mutex mBlockMutex;
    mutable std::shared_mutex mSharedMutex;

    std::mutex mBlocking;
    std::condition_variable mCondVar;

public:
    UGlobalQueue();
    ~UGlobalQueue();

    UTaskQueue *RegisterReactor(UReactor *reactor);

    UTaskQueue *FindByReactor(UReactor *reactor) const;
    UReactor *FindByQueue(UTaskQueue *queue) const;
};
