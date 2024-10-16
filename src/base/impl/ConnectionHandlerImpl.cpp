#include "ConnectionHandlerImpl.h"
#include "../GameWorld.h"
#include "../../system/protocol/ProtocolSystem.h"
#include "../../system/login/LoginSystem.h"

#include <spdlog/spdlog.h>

namespace base {
    void ConnectionHandlerImpl::OnConnected(const ConnectionPointer &conn) {

    }

    void ConnectionHandlerImpl::OnClosed(const ConnectionPointer &conn) {
        GetWorld().RemoveConnection(conn->GetKey());
    }

    awaitable<void> ConnectionHandlerImpl::OnReadPackageT(const ConnectionPointer &conn, Package *pkg) {
        if (!conn->GetContext().has_value()) {
            if (const auto sys = GetSystem<LoginSystem>(); sys != nullptr) {
                co_await sys->OnLogin(conn, pkg);
            }
            co_return;
        }

        const auto sys = GetSystem<ProtocolSystem>();
        if (sys == nullptr) {
            spdlog::error("{} - ProtocolSystem not found.", __func__);
            co_return;
        }

        co_await sys->OnReadPackage(conn, pkg);
    }
} // base