#include "LoginHandlerImpl.h"
#include "../../protobuf/ProtoType.generated.h"

#include <login.pb.cc>

namespace base {
    asio::awaitable<void> LoginHandlerImpl::OnPlayerLogin(const std::shared_ptr<Connection> &conn, const LoginInfo &info) {
        // TODO
        co_return;
    }

    LoginInfo LoginHandlerImpl::ParseLoginInfoT(Package *pkg) {
        if (pkg->GetID() != static_cast<uint32_t>(protocol::ProtoType::CS_Login_Request))
            return {};

        Login::CS_Login_Request request;
        request.ParseFromString(pkg->GetData());

        return {
            request.player_id(),
            request.token()
        };
    }
} // base