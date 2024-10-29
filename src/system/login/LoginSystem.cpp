#include "LoginSystem.h"

#include <spdlog/spdlog.h>

#include "../../base/Connection.h"
#include "../impl/LoginHandlerImpl.h"

namespace base {
    void LoginSystem::Init() {
        handler_ = std::make_unique<LoginHandlerImpl>();
    }

    bool LoginSystem::VerifyAddress(const asio::ip::address &addr) {
        // TODO
        return true;
    }

    uint64_t LoginSystem::VerifyToken(uint64_t pid, const std::string &token) {
        // TODO
        return pid;
    }

    awaitable<void> LoginSystem::OnLogin(const std::shared_ptr<UConnection> &conn, IPackage *pkg) {
        if (handler_ == nullptr)
            co_return;

        const auto info = handler_->ParseLoginInfo(pkg);
        if (info.pid == 0) {
            spdlog::warn("{} - Connection[{}] context is null but not receive the login request", conn->RemoteAddress().to_string(), __func__);
            co_return;
        }

        spdlog::trace("{} - Player id: {}, token: {}", __func__, info.pid, info.token);
        if (const auto pid = VerifyToken(info.pid, info.token); pid != 0) {
            conn->SetContext(std::make_any<uint64_t>(pid));
            co_await handler_->OnPlayerLogin(conn, info);
        }
    }
} // base
