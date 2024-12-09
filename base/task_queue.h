#pragma once


#include <functional>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

class UGlobalQueue;
class UReactor;

using AReactorTask = std::function<void(UReactor*)>;


class UTaskQueue final {

    UGlobalQueue *mGlobal;
    UReactor *mReactor;

    std::queue<AReactorTask> *mCurrentQueue;
    std::queue<AReactorTask> *mWaitingQueue;

    std::mutex mMutex;
    mutable std::shared_mutex mSharedMutex;

    std::atomic_bool bRunning;
    std::atomic_bool bInGlobal;

public:
    UTaskQueue() = delete;

    UTaskQueue(UGlobalQueue *global, UReactor *reactor);
    ~UTaskQueue();

    [[nodiscard]] UReactor *GetReactor() const;

    void PushTask(const AReactorTask &task);

    [[nodiscard]] bool IsEmpty() const;
    [[nodiscard]] bool IsRunning() const;

    void OnAddToGlobal();
    void OnRemoveFromGlobal();

    [[nodiscard]] bool IsInGlobal() const;

    void OnStart();
    void OnStop();
};
