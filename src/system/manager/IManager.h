#pragma once

#include "../../common/common.h"
#include "../../base/RepeatedTimer.h"
#include "../../common/utils.h"

#include <spdlog/spdlog.h>

namespace base {
    class IManager {

        asio::io_context &ctx_;
        AThreadID tid_;

        std::unordered_map<uint64_t, URepeatedTimer> timerMap_;

    public:
        IManager() = delete;

        explicit IManager(asio::io_context &ctx);
        virtual ~IManager() = default;

        void SetThreadID(AThreadID tid);
        [[nodiscard]] AThreadID GetThreadID() const;
        [[nodiscard]] bool IsSameThread() const;

        DISABLE_COPY_MOVE(IManager)

        virtual void OnTick(ATimePoint now);

        [[nodiscard]] asio::io_context &GetIOContext() const;

        template<typename FUNC, typename ... ARGS>
        void RunInThread(FUNC &&func, ARGS &&... args) {
            co_spawn(ctx_, [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
                try {
                    std::invoke(func, args...);
                } catch (std::exception &e) {
                    spdlog::error("IManager::RunInThread: {}", e.what());
                }
                co_return;
            }, detached);
        }

        template<typename FUNC, typename... ARGS>
        uint64_t SetTimer(const std::chrono::duration<uint32_t> expire, const bool repeat, FUNC &&func, ARGS &&... args) {
            const uint64_t timerID = CurrentTimeCount();
            if (auto [iter, res] = timerMap_.insert_or_assign(timerID, ctx_); res) {
                iter->second
                        .SetTimerID(timerID)
                        .SetExpireTime(expire)
                        .SetLoop(repeat)
                        .SetCallback(std::forward<FUNC>(func), std::forward<ARGS>(args)...);

                iter->second.Start();
                return timerID;
            } else
                return 0;
        }

        void StopTimer(uint64_t timerID);
    };

    template<typename T>
    concept MANAGER_TYPE = std::derived_from<T, IManager>;

} // base
