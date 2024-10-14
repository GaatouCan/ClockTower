#include "ConnectionHandlerImpl.h"

#include "../GameWorld.h"

namespace base {
    void ConnectionHandlerImpl::onConnected(const ConnectionPointer &conn) {

    }

    void ConnectionHandlerImpl::onClosed(const ConnectionPointer &conn) {
        GetWorld().removeConnection(conn->key());
    }

    awaitable<void> ConnectionHandlerImpl::onReadPackageT(const ConnectionPointer &conn, Package *pkg) {
        // TODO
        co_return;
    }
} // base