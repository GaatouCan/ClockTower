#include "LoginSystem.h"
#include "../../Connection.h"
#include "../base/GameWorld.h"

#include <spdlog/spdlog.h>

REGISTER_SUBSYSTEM(ULoginSystem, 2)

void ULoginSystem::Init() {

}

bool ULoginSystem::VerifyAddress(const asio::ip::address &addr) {
    // TODO
    return true;
}

uint64_t ULoginSystem::VerifyToken(uint64_t pid, const std::string &token) {
    // TODO
    return pid;
}

awaitable<void> ULoginSystem::OnLogin(const std::shared_ptr<UConnection> &conn, IPackage *pkg) {
    if (mHandler == nullptr) {
        spdlog::critical("{} - handler not set.", __func__);
        co_return;
    }

    const auto info = mHandler->ParseLoginInfo(pkg);
    if (info.pid == 0) {
        spdlog::warn("{} - Connection[{}] context is null but not receive the login request", conn->RemoteAddress().to_string(), __func__);
        co_return;
    }

    spdlog::trace("{} - Player id: {}, token: {}", __func__, info.pid, info.token);
    if (const auto pid = VerifyToken(info.pid, info.token); pid != 0) {
        conn->SetContext(std::make_any<uint64_t>(pid));
        co_await mHandler->OnPlayerLogin(conn, info);
    }
}
