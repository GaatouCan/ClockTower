#include <spdlog/spdlog.h>

#include "../base/GameWorld.h"
#include "../base/system/config/ConfigSystem.h"
#include "../base/system/protocol/ProtocolSystem.h"
#include "../base/system/manager/ManagerSystem.h"

#include "impl/ConnectionHandlerImpl.h"

#include "common/proto_def.h"
#include "common/mgr_def.h"


auto main(int argc, char *argv[]) -> int {
    spdlog::set_level(spdlog::level::trace);
    spdlog::info("Welcome to ClockTower!");

    if (const auto sys = GetSystem<UConfigSystem>(); sys != nullptr) {
        sys->SetYAMLPath("../../config");
    }

    if (const auto sys = GetSystem<UProtocolSystem>(); sys != nullptr) {
        LoadProtocol(sys);
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