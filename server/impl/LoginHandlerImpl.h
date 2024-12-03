#pragma once

#include <LoginHandler.h>


class ULoginHandlerImpl final : public ILoginHandler {
public:
    awaitable<std::shared_ptr<IAbstractPlayer>> OnPlayerLogin(const std::shared_ptr<UConnection> &conn, const FLoginInfo &info) override;
    awaitable<FLoginInfo> ParseLoginInfo(IPackage *) override;
};
