#pragma once

#include <LoginHandler.h>


class ULoginHandlerImpl final : public ILoginHandler {
public:
    awaitable<void> OnPlayerLogin(const APlayerPointer &plr, const FLoginInfo &info) override;
    awaitable<FLoginInfo> ParseLoginInfo(IPackage *) override;
};
