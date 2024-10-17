#include "ProtocolHandlerImpl.h"
#include "../../base/Connection.h"
#include "../../system/manager/ManagerSystem.h"
#include "../../player/PlayerManager.h"

#include <spdlog/spdlog.h>

namespace base {
    awaitable<void> ProtocolHandlerImpl::Execute(const std::shared_ptr<Connection> &conn, IPackage *pkg) {
        if (!pkg->IsAvailable()) {
            spdlog::warn("{} - Package unavailable", __func__);
            co_return;
        }

        if (pkg->GetID() >= static_cast<uint32_t>(ProtoType::PROTO_TYPE_MAX)) {
            spdlog::warn("{} - Protocol type out of range", __func__);
            co_return;
        }

        const auto type = static_cast<ProtoType>(pkg->GetID());

        auto func = Find(type);
        if (!func) {
            spdlog::warn("{} - Protocol type not found", __func__);
            co_return;
        }

        const auto plrMgr = GetManager<PlayerManager>();
        if (plrMgr == nullptr) {
            spdlog::warn("{} - PlayerManager not found", __func__);
            co_return;
        }

        const auto pid = std::any_cast<uint64_t>(conn->GetContext());
        if (const auto plr = plrMgr->FindPlayer(pid); plr != nullptr) {
            co_await std::invoke(func, plr, dynamic_cast<Package *>(pkg));
        }
    }
} // base