#include "IManager.h"

namespace base {
    IManager::IManager(asio::io_context &ctx)
        : ctx_(ctx) {
    }

    void IManager::SetThreadID(const AThreadID tid) {
        tid_ = tid;
    }

    AThreadID IManager::GetThreadID() const {
        return tid_;
    }

    bool IManager::IsSameThread() const {
        return tid_ == std::this_thread::get_id();
    }

    void IManager::OnTick(ATimePoint now) {
    }

    asio::io_context & IManager::GetIOContext() const {
        return ctx_;
    }

    void IManager::StopTimer(const uint64_t timerID) {
        if (const auto iter = timerMap_.find(timerID); iter != timerMap_.end()) {
            iter->second.Stop();
            timerMap_.erase(iter);
        }
    }
} // base
