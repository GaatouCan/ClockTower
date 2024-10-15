#include "LoginHandlerImpl.h"

namespace base {
    asio::awaitable<void> LoginHandlerImpl::login(const std::shared_ptr<Connection> &conn, const LoginInfo &) {
    }

    LoginInfo LoginHandlerImpl::parseLoginInfoT(Package *) {
    }
} // base