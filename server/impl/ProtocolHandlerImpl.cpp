#include "ProtocolHandlerImpl.h"
#include "../player/Player.h"
#include "../player/PlayerManager.h"

#include <Connection.h>
#include <system/manager/ManagerSystem.h>

#include <spdlog/spdlog.h>


awaitable<void> UProtocolHandlerImpl::Execute(const AProtoFunctor &func, const std::shared_ptr<UConnection> &conn, IPackage *pkg) {
    const auto plrMgr = GetManager<UPlayerManager>();
    if (plrMgr == nullptr) {
        spdlog::critical("{} - PlayerManager not found", __FUNCTION__);
        co_return;
    }

    const auto pid = std::any_cast<FPlayerID>(conn->GetContext());
    if (const auto plr = plrMgr->FindPlayer(pid); plr != nullptr) {
        try {
            assert(plr->GetConnection() == conn);
            co_await std::invoke(func, plr, pkg);
        } catch (std::exception &e) {
            spdlog::error("{} - Player[{}] Invoke Protocol Handler Error: {}", __FUNCTION__, plr->GetFullID(), e.what());
        }
    } else {
        spdlog::warn("{} - Player[{}] Not Found", __FUNCTION__, pid.ToUInt64());
    }
}
