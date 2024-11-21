#include "ProtocolHandlerImpl.h"

#include <Connection.h>
#include <system/manager/ManagerSystem.h>
#include <manager/player/PlayerManager.h>

#include <spdlog/spdlog.h>


void UProtocolHandlerImpl::Execute(const AProtoFunctor &func, const std::shared_ptr<UConnection> &conn, IPackage *pkg) {
    const auto plrMgr = GetManager<UPlayerManager>();
    if (plrMgr == nullptr) {
        spdlog::warn("{} - PlayerManager not found", __FUNCTION__);
        return;
    }

    const auto pid = std::any_cast<uint64_t>(conn->GetContext());
    if (const auto plr = plrMgr->FindPlayer(pid); plr != nullptr) {
        assert(plr->GetConnection() == conn);
        std::invoke(func, plr, pkg);
    }
}
