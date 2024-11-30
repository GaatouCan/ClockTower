#include "LoginSystem.h"
#include "../../Connection.h"
#include "../scene/SceneSystem.h"
#include "../scene/Scene.h"
#include "../scene/AbstractPlayer.h"
#include "../../GameWorld.h"

#include <spdlog/spdlog.h>


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
        spdlog::critical("{} - handler not set.", __FUNCTION__);
        co_return;
    }

    const auto info = co_await mHandler->ParseLoginInfo(pkg);
    if (info.pid == 0) {
        spdlog::warn("{} - Connection[{}] context is null but not receive the login request", conn->RemoteAddress().to_string(), __FUNCTION__);
        co_return;
    }

    spdlog::trace("{} - Player id: {}, token: {}", __FUNCTION__, info.pid, info.token);
    if (const auto pid = VerifyToken(info.pid, info.token); pid != 0) {
        conn->SetContext(std::make_any<uint64_t>(pid));

        if (const auto sys = GetSystem<USceneSystem>(); sys != nullptr) {
            if (const auto scene = sys->GetMainScene(); scene != nullptr) {
                if (const auto plr = scene->CreatePlayer(conn); plr != nullptr) {
                    plr->OnLogin();
                }
            }
        }

        co_await mHandler->OnPlayerLogin(conn, info);
    }
}
