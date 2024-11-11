#pragma once

#include "../../common.h"
#include "../../RepeatedTimer.h"
#include "../../utils.h"

#include <spdlog/spdlog.h>

class IManager {

    asio::io_context &mIOContext;
    AThreadID mThreadId;

    std::unordered_map<ATimerID, URepeatedTimer> mTimerMap;

protected:

    friend class UManagerSystem;

    explicit IManager(asio::io_context &ctx);
    virtual ~IManager() = default;

public:
    IManager() = delete;

    [[nodiscard]] virtual const char *GetManagerName() const = 0;

    void SetThreadID(AThreadID tid);
    [[nodiscard]] AThreadID GetThreadID() const;

    [[nodiscard]] bool IsSameThread() const;

    DISABLE_COPY_MOVE(IManager)

    virtual void OnTick(ATimePoint now);

    [[nodiscard]] asio::io_context &GetIOContext() const;

    template<typename Functor, typename... Args>
    void RunInThread(Functor &&func, Args &&... args) {
        co_spawn(mIOContext, [func = std::forward<Functor>(func), ...args = std::forward<Args>(args)]() -> awaitable<void> {
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
        if (auto [iter, res] = mTimerMap.insert_or_assign(timerID, mIOContext); res) {
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

#define MANAGER_BODY(mgr) \
    friend UManagerSystem; \
public: \
    [[nodiscard]] constexpr const char * GetManagerName() const override { \
        return #mgr; \
    } \
private: