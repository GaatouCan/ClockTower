#include "ConnectionHandlerImpl.h"

#include "../common/ProtoType.h"
#include "../player/PlayerManager.h"

#include <GameWorld.h>
#include <system/protocol/ProtocolSystem.h>
#include <system/login/LoginSystem.h>
#include <system/manager/ManagerSystem.h>


void UConnectionHandlerImpl::OnConnected() {
}

void UConnectionHandlerImpl::OnClosed() {
    GetWorld().RemoveConnection(mConn.lock()->GetKey());

    if (!mConn.lock()->GetContext().has_value())
        return;

    const auto pid = std::any_cast<FPlayerID>(mConn.lock()->GetContext());
    mConn.lock()->ResetContext();

    if (const auto plrMgr = GetManager<UPlayerManager>(); plrMgr != nullptr) {
        try {
            plrMgr->OnPlayerLogout(pid);
        } catch (std::exception &e) {
            spdlog::error("{} - {}", __FUNCTION__, e.what());
        }
    } else
        spdlog::error("{} - Fail to Found PlayerManager", __FUNCTION__);
}

awaitable<void> UConnectionHandlerImpl::OnReadPackage(IPackage *pkg) {
    spdlog::debug("{} - Receive Package[{}] From {}.", __FUNCTION__, ProtoTypeToString(static_cast<protocol::EProtoType>(pkg->GetID())), mConn.lock()->RemoteAddress().to_string());

    if (!mConn.lock()->GetContext().has_value()) {
        if (const auto sys = GetSystem<ULoginSystem>(); sys != nullptr) {
            co_await sys->OnLogin(mConn.lock(), pkg);
        } else {
            spdlog::critical("{}: LoginSystem not found.", __FUNCTION__);
            GetWorld().Shutdown();
            exit(-1);
        }
    } else {
        if (const auto sys = GetSystem<UProtocolSystem>(); sys != nullptr) {
            co_await sys->OnReadPackage(mConn.lock(), pkg);
        } else {
            spdlog::critical("{} - ProtocolSystem not found.", __FUNCTION__);
            GetWorld().Shutdown();
            exit(-1);
        }
    }
    co_return;
}
