#include <spdlog/spdlog.h>

#include "../base/GameWorld.h"
#include "../base/system/config/ConfigSystem.h"
#include "../base/system/protocol/ProtocolSystem.h"
#include "../base/system/login/LoginSystem.h"
#include "../base/system/manager/ManagerSystem.h"

#include "impl/ConnectionHandlerImpl.h"
#include "impl/ProtocolHandlerImpl.h"
#include "impl/LoginHandlerImpl.h"

#include "common/loader_def.h"
#include "common/proto_def.h"
#include "common/mgr_def.h"


auto main(int argc, char *argv[]) -> int {
    spdlog::set_level(spdlog::level::trace);
    spdlog::info("Welcome to ClockTower!");

    if (const auto sys = GetSystem<UConfigSystem>(); sys != nullptr) {
        sys->SetYAMLPath("../../config");
        sys->SetLoaderFunctor(&LoadConfigLoader);
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

    GetWorld().FilterConnection([](const AConnectionPointer &conn) {
        if (conn != nullptr) {
            conn->SetHandler<UConnectionHandlerImpl>();
        }
    });

    GetWorld().Init();
    GetWorld().Run();

    return 0;
}