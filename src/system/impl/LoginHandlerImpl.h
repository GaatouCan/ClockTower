#pragma once

#include "../ILoginHandler.h"
#include "../../base/impl/Package.h"

namespace base {
    class LoginHandlerImpl : public TLoginHandler<Package> {
    public:
        asio::awaitable<void> login(const std::shared_ptr<Connection> &conn, const LoginInfo &) override;
        LoginInfo parseLoginInfoT(Package *) override;
    };
} // base