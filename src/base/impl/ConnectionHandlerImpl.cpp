#include "ConnectionHandlerImpl.h"
#include "../GameWorld.h"
#include "../../system/protocol/ProtocolSystem.h"
#include "../../system/login/LoginSystem.h"
#include "../../player/PlayerManager.h"
#include "../../system/manager/ManagerSystem.h"

#include <spdlog/spdlog.h>

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

    void ConnectionHandlerImpl::OnReadPackageT(const ConnectionPointer &conn, Package *pkg) {
        if (!conn->GetContext().has_value()) {
            if (const auto sys = GetSystem<LoginSystem>(); sys != nullptr) {
                sys->OnLogin(conn, pkg);
            } else
                spdlog::error("{}: LoginSystem not found.", __func__);
        } else {
            if (const auto sys = GetSystem<ProtocolSystem>(); sys != nullptr) {
                sys->OnReadPackage(conn, pkg);
            } else
                spdlog::error("{} - ProtocolSystem not found.", __func__);
        }
    }
} // base