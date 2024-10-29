#pragma once

#include "../ILoginHandler.h"
#include "../../base/IPackage.h"
#include "../../base/ISubSystem.h"


class UConnection;

class LoginSystem final : public ISubSystem {
    SUB_SYSTEM_BODY(LoginSystem)
    void Init() override;

public:
    bool VerifyAddress(const asio::ip::address &addr);

    uint64_t VerifyToken(uint64_t pid, const std::string &token);

    awaitable<void> OnLogin(const std::shared_ptr<UConnection> &conn, IPackage *pkg);

private:
    std::unique_ptr<ILoginHandler> handler_;
};
