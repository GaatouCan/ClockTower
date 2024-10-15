#include "ConnectionHandlerImpl.h"
#include "../GameWorld.h"
#include "../../system/protocol/ProtocolSystem.h"

#include <spdlog/spdlog.h>

namespace base {
    void ConnectionHandlerImpl::onConnected(const ConnectionPointer &conn) {

    }

    void ConnectionHandlerImpl::onClosed(const ConnectionPointer &conn) {
        GetWorld().removeConnection(conn->key());
    }

    awaitable<void> ConnectionHandlerImpl::onReadPackageT(const ConnectionPointer &conn, Package *pkg) {
        if (!conn->context().has_value()) {
            // TODO: Login Logic
            co_return;
        }

        const auto sys = GetSystem<ProtocolSystem>();
        if (sys == nullptr) {
            spdlog::error("{} - ProtocolSystem not found.", __func__);
            co_return;
        }

        co_await sys->onReadPackage(conn, pkg);
    }
} // base