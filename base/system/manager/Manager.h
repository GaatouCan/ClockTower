#pragma once

#include "../../RepeatedTimer.h"
#include "../../utils.h"
#include "../../ContextNode.h"

#include <map>
#include <spdlog/spdlog.h>

class IManager {

    FContextNode &mContextNode;
    std::map<ATimerID, URepeatedTimer> mTimerMap;

public:
    IManager() = delete;

    explicit IManager(FContextNode &ctx);
    virtual ~IManager() = default;

    DISABLE_COPY_MOVE(IManager)

    [[nodiscard]] virtual const char *GetManagerName() const = 0;

    [[nodiscard]] AThreadID GetThreadID() const;

    [[nodiscard]] bool IsSameThread() const;

    virtual void OnTick(ATimePoint now);

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
    ATimerID SetTimer(const std::chrono::duration<uint32_t> expire, const bool repeat, Functor &&func, Args &&... args) {
        const uint64_t timerID = UnixTime();
        if (auto [iter, res] = mTimerMap.insert_or_assign(timerID, mContextNode.ctx); res) {
            iter->second
                    .SetTimerID(timerID)
                    .SetExpireTime(expire)
                    .SetLoop(repeat)
                    .SetCallback(std::forward<Functor>(func), std::forward<Args>(args)...);

            iter->second.Start();
            return timerID;
        }
        return 0;
    }

    void StopTimer(ATimerID timerID);
};

template<typename T>
concept MANAGER_TYPE = std::derived_from<T, IManager>;
