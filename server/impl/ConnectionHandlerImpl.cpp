#include "ConnectionHandlerImpl.h"

#include "../base/GameWorld.h"
#include "../base/system/protocol/ProtocolSystem.h"
#include "../base/system/login/LoginSystem.h"
#include "../base/system/manager/ManagerSystem.h"
#include "../player/PlayerManager.h"
#include "../common/ProtoType.h"


void UConnectionHandlerImpl::OnConnected(const AConnectionPointer &conn) {
}

void UConnectionHandlerImpl::OnClosed(const AConnectionPointer &conn) {
    GetWorld().RemoveConnection(conn->GetKey());

    if (!conn->GetContext().has_value())
        return;

    const auto pid = std::any_cast<uint64_t>(conn->GetContext());
    conn->ResetContext();

    if (const auto plrMgr = GetManager<UPlayerManager>(); plrMgr != nullptr)
        plrMgr->OnPlayerLogout(pid);
    else
        spdlog::error("{} - Fail to Found PlayerManager", __FUNCTION__);
}

awaitable<void> UConnectionHandlerImpl::OnReadPackage(const AConnectionPointer &conn, IPackage *pkg) {
    spdlog::trace("{} - Receive Package[{}] From {}.", __FUNCTION__, ProtoTypeToString(static_cast<protocol::EProtoType>(pkg->GetID())), conn->RemoteAddress().to_string());

    if (!conn->GetContext().has_value()) {
        if (const auto sys = GetSystem<ULoginSystem>(); sys != nullptr) {
            co_await sys->OnLogin(conn, pkg);
        } else {
            spdlog::critical("{}: LoginSystem not found.", __FUNCTION__);
            GetWorld().Shutdown();
            exit(-1);
        }
    } else {
        if (const auto sys = GetSystem<UProtocolSystem>(); sys != nullptr) {
            co_await sys->OnReadPackage(conn, pkg);
        } else {
            spdlog::critical("{} - ProtocolSystem not found.", __FUNCTION__);
            GetWorld().Shutdown();
            exit(-1);
        }
    }
}
