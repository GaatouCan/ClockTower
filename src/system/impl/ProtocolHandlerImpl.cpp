#include "ProtocolHandlerImpl.h"

namespace base {
    asio::awaitable<void> ProtocolHandlerImpl::execute(const std::shared_ptr<Connection> &conn, IPackage *pkg) {
        // TODO
        co_return;
    }
} // base