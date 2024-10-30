#pragma once

#include "../LoginHandler.h"
#include "../../base/impl/Package.h"


class ULoginHandlerImpl final : public ILoginHandler {
public:
    asio::awaitable<void> OnPlayerLogin(const std::shared_ptr<class UConnection> &conn, const FLoginInfo &) override;
    FLoginInfo ParseLoginInfo(IPackage *) override;
};
