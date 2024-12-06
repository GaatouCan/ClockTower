#include "LoginSystem.h"

#include "../../Connection.h"
#include "../../GameWorld.h"

#include "../scene/SceneSystem.h"
#include "../scene/Scene.h"

#include <spdlog/spdlog.h>


void ULoginSystem::Init() {

}

bool ULoginSystem::VerifyAddress(const asio::ip::address &addr) {
    // TODO
    return true;
}

FPlayerID ULoginSystem::VerifyToken(FPlayerID pid, const std::string &token) {
    // TODO
    return pid;
}

awaitable<void> ULoginSystem::OnLogin(const AConnectionPointer &conn, IPackage *pkg) {
    if (conn == nullptr || pkg == nullptr)
        co_return;

    if (mHandler == nullptr) {
        spdlog::critical("{} - handler not set.", __FUNCTION__);
        co_return;
    }

    try {
        const auto info = co_await mHandler->ParseLoginInfo(pkg);
        if (!info.pid.IsValid()) {
            spdlog::warn("{} - Connection[{}] context is null but not receive the login request", __FUNCTION__, conn->GetKey());
            co_return;
        }

        spdlog::trace("{} - Player id: {}, token: {}", __FUNCTION__, info.pid.ToUInt64(), info.token);
        if (const auto pid = VerifyToken(info.pid, info.token); pid.IsValid()) {
            conn->SetContext(std::make_any<FPlayerID>(pid));

            if (const auto plr = co_await mHandler->OnPlayerLogin(conn, info); plr != nullptr) {
                if (const auto sys = GetSystem<USceneSystem>(); sys != nullptr) {
                    if (const auto scene = sys->GetMainScene(); scene != nullptr) {
                        scene->PlayerEnterScene(plr);
                    }
                }
            } else {
                conn->ResetContext();
                conn->Disconnect();
            }
        }
    } catch (const std::exception &e) {
        spdlog::error("{} - {}", __FUNCTION__, e.what());

        conn->ResetContext();
        conn->Disconnect();
    }
}
