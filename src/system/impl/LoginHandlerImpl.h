#pragma once

#include "../ILoginHandler.h"
#include "../../base/impl/Package.h"

namespace base {
    class LoginHandlerImpl final : public TLoginHandler<FPackage> {
    public:
        asio::awaitable<void> OnPlayerLogin(const std::shared_ptr<Connection> &conn, const LoginInfo &) override;
        LoginInfo ParseLoginInfoT(FPackage *) override;
    };
} // base
