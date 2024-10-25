#include "ConnectionHandlerImpl.h"
#include "../GameWorld.h"
#include "../../system/protocol/ProtocolSystem.h"
#include "../../system/login/LoginSystem.h"
#include "../../player/PlayerManager.h"
#include "../../system/manager/ManagerSystem.h"

namespace base {
    void ConnectionHandlerImpl::OnConnected(const ConnectionPointer &conn) {

    }

    void ConnectionHandlerImpl::OnClosed(const ConnectionPointer &conn) {
        GetWorld().RemoveConnection(conn->GetKey());

        if (!conn->GetContext().has_value())
            return;

        const auto pid = std::any_cast<uint64_t>(conn->GetContext().has_value());
        conn->ResetContext();

        if (const auto plrMgr = GetManager<PlayerManager>(); plrMgr != nullptr)
            plrMgr->OnPlayerLogout(pid);
    }

    awaitable<void> ConnectionHandlerImpl::OnReadPackageT(const ConnectionPointer &conn, Package *pkg) {
        if (!conn->GetContext().has_value()) {
            if (const auto sys = GetSystem<LoginSystem>(); sys != nullptr) {
                co_await sys->OnLogin(conn, pkg);
            } else {
                spdlog::error("{}: LoginSystem not found.", __func__);
                GetWorld().Shutdown();
                exit(-1);
            }
        } else {
            if (const auto sys = GetSystem<ProtocolSystem>(); sys != nullptr) {
                co_await sys->OnReadPackage(conn, pkg);
            } else {
                spdlog::error("{} - ProtocolSystem not found.", __func__);
                GetWorld().Shutdown();
                exit(-1);
            }
        }
    }
} // base