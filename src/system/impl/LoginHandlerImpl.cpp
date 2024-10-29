#include "LoginHandlerImpl.h"
#include "../../protobuf/ProtoType.generated.h"
#include "../../player/PlayerManager.h"
#include "../manager/ManagerSystem.h"

#include <login.pb.h>
#include <spdlog/spdlog.h>


awaitable<void> LoginHandlerImpl::OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FLoginInfo &info) {
    if (const auto plrMgr = GetManager<PlayerManager>(); plrMgr != nullptr) {
        co_await plrMgr->OnPlayerLogin(conn, info.pid);
    } else
        spdlog::warn("{} - PlayerManager not found", __func__);
}

FLoginInfo LoginHandlerImpl::ParseLoginInfo(IPackage *pkg) {
    try {
        const auto tmp = dynamic_cast<FPackage *>(pkg);

        if (pkg->GetID() != static_cast<uint32_t>(protocol::ProtoType::CS_LoginRequest))
            return {};

        Login::CS_LoginRequest request;
        request.ParseFromString(tmp->GetData());

        return {
            request.player_id(),
            request.token()
        };
    } catch (std::exception &e) {
        spdlog::warn("{} - {}", __func__, e.what());
        return {};
    }
}
