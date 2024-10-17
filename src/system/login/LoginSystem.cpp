#include "LoginSystem.h"

namespace base {
    void LoginSystem::Init() {
    }

    bool LoginSystem::VerifyAddress(const asio::ip::address& addr) {
        // TODO
        return true;
    }

    uint64_t LoginSystem::VerifyToken(uint64_t pid, const std::string &token) {
        // TODO
        return pid;
    }

    awaitable<void> LoginSystem::OnLogin(const std::shared_ptr<Connection> &conn, IPackage *pkg) {
        if (handler_ == nullptr)
            co_return;

        const auto info = handler_->ParseLoginInfo(pkg);

        if (const auto pid = VerifyToken(info.pid, info.token); pid != 0) {
            co_await handler_->OnPlayerLogin(conn, info);
        }
    }
} // base