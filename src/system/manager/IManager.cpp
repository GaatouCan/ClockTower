#include "IManager.h"

namespace base {
    IManager::IManager(asio::io_context &ctx)
        : ctx_(ctx) {
    }

    asio::io_context & IManager::getIOContext() const {
        return ctx_;
    }
} // base
