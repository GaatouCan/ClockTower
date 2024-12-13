#include "ProtocolHandlerImpl.h"
#include "../player/Player.h"
#include "../player/PlayerManager.h"

#include <connection.h>
#include <system/manager/manager_system.h>

#include <spdlog/spdlog.h>


awaitable<void> UProtocolHandlerImpl::Execute(const AProtoFunctor &func, const std::shared_ptr<UConnection> &conn, IPackage *pkg) {
    const auto plrMgr = GetManager<UPlayerManager>();
    if (plrMgr == nullptr) {
        spdlog::critical("{} - PlayerManager not found", __FUNCTION__);
        co_return;
    }

    const auto pid = std::any_cast<FPlayerID>(conn->GetContext());
    if (const auto plr = plrMgr->FindPlayer(pid.localID); plr != nullptr) {
        try {
            assert(plr->GetConnection() == conn);
            co_await std::invoke(func, plr, pkg);
        } catch (std::exception &e) {
            spdlog::error("{} - Player[{}] Invoke Protocol Handler Error: {}", __FUNCTION__, plr->GetLocalID(), e.what());
        }
    } else {
        spdlog::warn("{} - Player[{}] Not Found", __FUNCTION__, pid.GetLocalID());
    }
}
