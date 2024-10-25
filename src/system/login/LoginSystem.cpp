#include "LoginSystem.h"

#include <spdlog/spdlog.h>

#include "../../base/Connection.h"
#include "../impl/LoginHandlerImpl.h"

namespace base {
    void LoginSystem::Init() {
        handler_ = std::make_unique<LoginHandlerImpl>();
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
        spdlog::debug("{} - PlayerID: {}, Token: {}",__func__, info.pid, info.token);

        if (const auto pid = VerifyToken(info.pid, info.token); pid != 0) {
            conn->SetContext(std::make_any<uint64_t>(pid));
            co_await handler_->OnPlayerLogin(conn, info);
        }
    }
} // base