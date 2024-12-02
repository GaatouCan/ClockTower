#include "LoginHandlerImpl.h"
#include "../common/ProtoType.h"
#include "../player/PlayerManager.h"

#include <impl/Package.h>
#include <system/manager/ManagerSystem.h>

#include <login.pb.h>
#include <spdlog/spdlog.h>


awaitable<void> ULoginHandlerImpl::OnPlayerLogin(const APlayerPointer &plr, const FLoginInfo &info) {
    if (const auto plrMgr = GetManager<UPlayerManager>(); plrMgr != nullptr) {
        // FIXME
        // co_await plrMgr->OnPlayerLogin(conn, info.pid);
        co_return;
    } else
        spdlog::warn("{} - PlayerManager not found", __FUNCTION__);
}

awaitable<FLoginInfo> ULoginHandlerImpl::ParseLoginInfo(IPackage *pkg) {
    try {
        const auto tmp = dynamic_cast<FPackage *>(pkg);

        if (pkg->GetID() != static_cast<uint32_t>(protocol::EProtoType::CS_LoginRequest))
            co_return FLoginInfo{};

        Login::CS_LoginRequest request;
        request.ParseFromString(tmp->GetData());

        FLoginInfo info;

        info.pid.FromUInt64(request.player_id());
        info.token = request.token();

        co_return info;
    } catch (std::exception &e) {
        spdlog::warn("{} - {}", __FUNCTION__, e.what());
        co_return FLoginInfo{};
    }
}
