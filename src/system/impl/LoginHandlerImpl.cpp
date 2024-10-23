#include "LoginHandlerImpl.h"
#include "../../protobuf/ProtoType.generated.h"
#include "../../player/PlayerManager.h"
#include "../manager/ManagerSystem.h"

#include <login.pb.h>
#include <spdlog/spdlog.h>


namespace base {
    awaitable<void> LoginHandlerImpl::OnPlayerLogin(const std::shared_ptr<Connection> &conn, const LoginInfo &info) {
        if (const auto plrMgr = GetManager<PlayerManager>(); plrMgr != nullptr) {
            co_await plrMgr->OnPlayerLogin(conn, info.pid);
        } else
            spdlog::info("{} - PlayerManager not found", __func__);
    }

    LoginInfo LoginHandlerImpl::ParseLoginInfoT(Package *pkg) {
        if (pkg->GetID() != static_cast<uint32_t>(protocol::ProtoType::CS_LoginRequest))
            return {};

        Login::CS_LoginRequest request;
        request.ParseFromString(pkg->GetData());

        return {
            request.player_id(),
            request.token()
        };
    }
} // base