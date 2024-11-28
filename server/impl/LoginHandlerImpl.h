#pragma once

#include <LoginHandler.h>


class ULoginHandlerImpl final : public ILoginHandler {
public:
    awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FLoginInfo &) override;
    awaitable<FLoginInfo> ParseLoginInfo(IPackage *) override;
};
