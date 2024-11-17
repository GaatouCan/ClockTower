#pragma once

#include <LoginHandler.h>


class ULoginHandlerImpl final : public ILoginHandler {
public:
    asio::awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FLoginInfo &) override;
    FLoginInfo ParseLoginInfo(IPackage *) override;
};
