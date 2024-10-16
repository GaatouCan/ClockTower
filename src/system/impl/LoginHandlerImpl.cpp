#include "LoginHandlerImpl.h"
#include "../../protobuf/ProtoType.generated.h"

#include <login.pb.cc>

namespace base {
    asio::awaitable<void> LoginHandlerImpl::login(const std::shared_ptr<Connection> &conn, const LoginInfo &info) {
        // TODO
        co_return;
    }

    LoginInfo LoginHandlerImpl::parseLoginInfoT(Package *pkg) {
        if (pkg->id() != static_cast<uint32_t>(protocol::ProtoType::CS_Login_Request))
            return {};

        Login::CS_Login_Request request;
        request.ParseFromString(pkg->data());

        return {
            request.player_id(),
            request.token()
        };
    }
} // base