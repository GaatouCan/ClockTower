#pragma once

#include "../../RepeatedTimer.h"
#include "../../utils.h"
#include "../../ContextNode.h"

#include <map>
#include <mutex>
#include <shared_mutex>
#include <spdlog/spdlog.h>

class IManager {

    FContextNode &mContextNode;

    std::map<FUniqueID, URepeatedTimer> mTimerMap;
    std::mutex mTimerMutex;
    std::shared_mutex mTimerSharedMutex;

public:
    IManager() = delete;

    explicit IManager(FContextNode &ctx);
    virtual ~IManager() = default;

    DISABLE_COPY_MOVE(IManager)

    [[nodiscard]] virtual const char *GetManagerName() const = 0;

    [[nodiscard]] AThreadID GetThreadID() const;

    [[nodiscard]] bool IsSameThread() const;

    virtual void OnTick(ATimePoint now);
    virtual void OnDayChange();

    [[nodiscard]] AIOContext &GetIOContext() const;

    [[nodiscard]] IPackage *BuildPackage() const;

    template<typename Functor, typename... Args>
    void RunInThread(Functor &&func, Args &&... args) {
        co_spawn(mContextNode.ctx, [func = std::forward<Functor>(func), ...args = std::forward<Args>(args)]() -> awaitable<void> {
            try {
                std::invoke(func, args...);
            } catch (std::exception &e) {
                spdlog::error("IManager::RunInThread: {}", e.what());
            }
            co_return;
        }, detached);
    }

    template<typename Functor, typename... Args>
    FUniqueID SetTimer(const std::chrono::duration<uint32_t> expire, const bool repeat, Functor &&func, Args &&... args) {
        FUniqueID timerID = FUniqueID::RandGenerate();
        {
            std::shared_lock lock(mTimerSharedMutex);
            while (mTimerMap.contains(timerID)) {
                timerID = FUniqueID::RandGenerate();
            }
        }

        {
            std::scoped_lock lock(mTimerMutex);
            if (auto [iter, res] = mTimerMap.insert_or_assign(timerID, mContextNode.ctx); res) {
                iter->second
                        .SetTimerID(timerID)
                        .SetExpireTime(expire)
                        .SetLoop(repeat)
                        .SetCallback(std::forward<Functor>(func), std::forward<Args>(args)...);

                iter->second.Start();
                return timerID;
            }
        }
        return {};
    }

    void StopTimer(FUniqueID timerID);
    void CleanAllTimer();
};

template<typename T>
concept MANAGER_TYPE = std::derived_from<T, IManager>;
