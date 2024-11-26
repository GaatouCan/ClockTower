#include <GameWorld.h>

#include <system/protocol/ProtocolSystem.h>
#include <system/login/LoginSystem.h>

#include <manager/player/PlayerManager.h>

#include "impl/ConnectionHandlerImpl.h"
#include "impl/ProtocolHandlerImpl.h"
#include "impl/LoginHandlerImpl.h"
#include "player/Player.h"

#include "common/config_def.h"
#include "common/proto_def.h"
#include "common/logger_def.h"
#include "common/manager_def.h"

auto main(int argc, char *argv[]) -> int {
    spdlog::set_level(spdlog::level::trace);
    spdlog::info("Welcome To ClockTower!");

    if (const auto sys = GetSystem<UConfigSystem>(); sys != nullptr) {
#ifdef WIN32
        sys->SetYAMLPath("../../config");
#else
        sys->SetYAMLPath("../config");
#endif
        sys->SetLogicConfigLoader(&LoadLogicConfig);
    }

    if (const auto sys = GetSystem<UProtocolSystem>(); sys != nullptr) {
        LoadProtocol(sys);
        sys->SetHandler<UProtocolHandlerImpl>();
    }

    if (const auto sys = GetSystem<ULoginSystem>(); sys != nullptr) {
        sys->SetHandler<ULoginHandlerImpl>();
    }

    if (const auto sys = GetSystem<UManagerSystem>(); sys != nullptr) {
        sys->SetManagerLoader(&LoadManager);
    }

    UPlayerManager::SetPlayerCreator(&CreatePlayer);

    GetWorld().DefineLogger(&InitLogger);

    GetWorld().FilterConnection([](const AConnectionPointer &conn) {
        if (conn != nullptr) {
            conn->SetHandler<UConnectionHandlerImpl>();
        }
    });

    GetWorld().Init();
    GetWorld().Run();

    return 0;
}