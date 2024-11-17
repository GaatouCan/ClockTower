#include "LoginHandlerImpl.h"
#include "../common/ProtoType.h"
#include "../player/PlayerManager.h"

#include <impl/Package.h>
#include <system/manager/ManagerSystem.h>

#include <login.pb.h>
#include <spdlog/spdlog.h>


awaitable<void> ULoginHandlerImpl::OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FLoginInfo &info) {
    if (const auto plrMgr = GetManager<UPlayerManager>(); plrMgr != nullptr) {
        co_await plrMgr->OnPlayerLogin(conn, info.pid);
    } else
        spdlog::warn("{} - PlayerManager not found", __FUNCTION__);
}

FLoginInfo ULoginHandlerImpl::ParseLoginInfo(IPackage *pkg) {
    try {
        const auto tmp = dynamic_cast<FPackage *>(pkg);

        if (pkg->GetID() != static_cast<uint32_t>(protocol::EProtoType::CS_LoginRequest))
            return {};

        Login::CS_LoginRequest request;
        request.ParseFromString(tmp->GetData());

        return {
            request.player_id(),
            request.token()
        };
    } catch (std::exception &e) {
        spdlog::warn("{} - {}", __FUNCTION__, e.what());
        return {};
    }
}
