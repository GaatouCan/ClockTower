#include "ProtocolHandlerImpl.h"
#include "../../base/Connection.h"
#include "../../system/manager/ManagerSystem.h"
#include "../../player/PlayerManager.h"

#include <spdlog/spdlog.h>


awaitable<void> ProtocolHandlerImpl::Execute(const AProtoFunctor &func, const std::shared_ptr<UConnection> &conn, IPackage *pkg) {
    const auto plrMgr = GetManager<UPlayerManager>();
    if (plrMgr == nullptr) {
        spdlog::warn("{} - PlayerManager not found", __func__);
        co_return;
    }

    const auto pid = std::any_cast<uint64_t>(conn->GetContext());
    if (const auto plr = plrMgr->FindPlayer(pid); plr != nullptr) {
        assert(plr->GetConnection() == conn);
        co_await std::invoke(func, plr, pkg);
    }
}
