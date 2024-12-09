#pragma once


#include <functional>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>


using AReactorTask = std::function<void(class UReactor*)>;


class UTaskQueue final {

    class UGlobalQueue *mGlobal;

    std::queue<AReactorTask> *mCurrentQueue;
    std::queue<AReactorTask> *mWaitingQueue;

    std::mutex mMutex;
    mutable std::shared_mutex mSharedMutex;

    std::atomic_bool bRunning;
    std::atomic_bool bInGlobal;

public:
    UTaskQueue() = delete;

    explicit UTaskQueue(UGlobalQueue *global);
    ~UTaskQueue();

    void PushTask(const AReactorTask &task);

    [[nodiscard]] bool IsEmpty() const;
    [[nodiscard]] bool IsRunning() const;

    void OnAddToGlobal();
    [[nodiscard]] bool IsInGlobal() const;

    void OnStart();
    void OnStop();
};
