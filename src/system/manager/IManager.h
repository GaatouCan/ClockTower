#pragma once

#include "../../common/common.h"

namespace base {
    class IManager {

        asio::io_context &ctx_;
        ThreadID tid_;

    public:
        IManager() = delete;

        explicit IManager(asio::io_context &ctx);
        virtual ~IManager() = default;

        void SetThreadID(ThreadID tid);
        [[nodiscard]] ThreadID GetThreadID() const;
        [[nodiscard]] bool IsSameThread() const;

        DISABLE_COPY_MOVE(IManager)

        [[nodiscard]] asio::io_context &GetIOContext() const;

        template<typename FUNC, typename ... ARGS>
        void RunInThread(FUNC &&func, ARGS &&... args) {
            co_spawn(ctx_, [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
                std::invoke(func, args...);
                co_return;
            }, detached);
        }
    };

    template<typename T>
    concept MANAGER_TYPE = std::derived_from<T, IManager>;

} // base
