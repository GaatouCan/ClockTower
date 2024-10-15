#pragma once

#include "../ILoginHandler.h"
#include "../../base/IPackage.h"
#include "../../base/ISubSystem.h"

namespace base {

    class Connection;

    class LoginSystem final : public ISubSystem {

        SUB_SYSTEM_BODY(LoginSystem)
        void init() override;

    public:
        bool verifyAddress(const std::shared_ptr<Connection> &conn);
        uint64_t verifyToken(uint64_t pid, const std::string &token);

        awaitable<void> onLogin(const std::shared_ptr<Connection> &conn, IPackage *pkg);

    private:
        std::unique_ptr<ILoginHandler> handler_;
    };
} // base
