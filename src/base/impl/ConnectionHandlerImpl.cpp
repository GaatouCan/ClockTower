#include "ConnectionHandlerImpl.h"
#include "../GameWorld.h"
#include "../../system/protocol/ProtocolSystem.h"
#include "../../system/login/LoginSystem.h"
#include "../../player/PlayerManager.h"
#include "../../system/manager/ManagerSystem.h"


void UConnectionHandlerImpl::OnConnected(const AConnectionPointer &conn) {
}

void UConnectionHandlerImpl::OnClosed(const AConnectionPointer &conn) {
    GetWorld().RemoveConnection(conn->GetKey());

    if (!conn->GetContext().has_value())
        return;

    const auto pid = std::any_cast<uint64_t>(conn->GetContext());
    conn->ResetContext();

    if (const auto plrMgr = GetManager<PlayerManager>(); plrMgr != nullptr)
        plrMgr->OnPlayerLogout(pid);
    else
        spdlog::error("{} - Fail to Found PlayerManager", __func__);
}

awaitable<void> UConnectionHandlerImpl::OnReadPackage(const AConnectionPointer &conn, IPackage *pkg) {
    spdlog::trace("{} - Receive Package[{}] From {}.", __func__, protocol::ProtoTypeToString(static_cast<protocol::ProtoType>(pkg->GetID())), conn->RemoteAddress().to_string());

    if (!conn->GetContext().has_value()) {
        if (const auto sys = GetSystem<LoginSystem>(); sys != nullptr) {
            co_await sys->OnLogin(conn, pkg);
        } else {
            spdlog::critical("{}: LoginSystem not found.", __func__);
            GetWorld().Shutdown();
            exit(-1);
        }
    } else {
        if (const auto sys = GetSystem<ProtocolSystem>(); sys != nullptr) {
            co_await sys->OnReadPackage(conn, pkg);
        } else {
            spdlog::critical("{} - ProtocolSystem not found.", __func__);
            GetWorld().Shutdown();
            exit(-1);
        }
    }
}
