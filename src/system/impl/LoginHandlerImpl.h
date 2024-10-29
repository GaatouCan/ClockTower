#pragma once

#include "../ILoginHandler.h"
#include "../../base/impl/Package.h"


class LoginHandlerImpl final : public ILoginHandler {
public:
    asio::awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FLoginInfo &) override;
    FLoginInfo ParseLoginInfo(IPackage *) override;
};
