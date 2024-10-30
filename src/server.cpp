#include <spdlog/spdlog.h>

#include "base/GameWorld.h"
#include "system/config/ConfigSystem.h"
#include "system/protocol/ProtocolSystem.h"
#include "system/manager/ManagerSystem.h"
#include "common/mgrdef.h"
#include <Protocol.gen.h>

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
        LoadManager(sys);
    }

    GetWorld().Init();
    GetWorld().Run();

    return 0;
}