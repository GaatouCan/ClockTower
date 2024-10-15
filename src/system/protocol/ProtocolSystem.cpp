#include "ProtocolSystem.h"
#include "../impl/ProtocolHandlerImpl.h"

namespace base {
    void ProtocolSystem::init() {
        setHandler<ProtocolHandlerImpl>();
        registerProtocol();
    }

    awaitable<void> ProtocolSystem::onReadPackage(const std::shared_ptr<Connection> &conn, IPackage *pkg) const {
        if (handler_ != nullptr) {
            co_await handler_->execute(conn, pkg);
        }
        co_return;
    }
} // base