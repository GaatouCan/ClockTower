#include "IManager.h"

namespace base {
    IManager::IManager(asio::io_context &ctx)
        : ctx_(ctx) {
    }

    void IManager::SetThreadID(const ThreadID tid) {
        tid_ = tid;
    }

    ThreadID IManager::GetThreadID() const {
        return tid_;
    }

    bool IManager::IsSameThread() const {
        return tid_ == std::this_thread::get_id();
    }

    asio::io_context & IManager::GetIOContext() const {
        return ctx_;
    }
} // base
