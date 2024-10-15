#include "LoginSystem.h"

namespace base {
    void LoginSystem::init() {
    }

    bool LoginSystem::verifyAddress(const std::shared_ptr<Connection> &conn) {
        return true;
    }

    uint64_t LoginSystem::verifyToken(uint64_t pid, const std::string &token) {
        return pid;
    }

    awaitable<void> LoginSystem::onLogin(const std::shared_ptr<Connection> &conn, IPackage *pkg) {
        if (handler_ == nullptr)
            co_return;

        const auto info = handler_->parseLoginInfo(pkg);
        const auto pid = verifyToken(info.pid, info.token);

        if (pid != 0) {
            co_await handler_->login(conn, info);
        }
    }
} // base