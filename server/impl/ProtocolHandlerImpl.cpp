#include "ProtocolHandlerImpl.h"
#include "../player/Player.h"
#include "../player/PlayerManager.h"

#include <Connection.h>
#include <system/manager/ManagerSystem.h>

#include <spdlog/spdlog.h>


awaitable<void> UProtocolHandlerImpl::Execute(const AProtoFunctor &func, const std::shared_ptr<UConnection> &conn, IPackage *pkg) {
    const auto plrMgr = GetManager<UPlayerManager>();
    if (plrMgr == nullptr) {
        spdlog::warn("{} - PlayerManager not found", __FUNCTION__);
        co_return;
    }

    const auto pid = std::any_cast<FPlayerID>(conn->GetContext());
    if (const auto plr = plrMgr->FindPlayer(pid); plr != nullptr) {
        assert(plr->GetConnection() == conn);
        co_await std::invoke(func, plr, pkg);
    }
}
