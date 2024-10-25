#include <spdlog/spdlog.h>

#include "base/GameWorld.h"
#include "system/config/ConfigSystem.h"

auto main(int argc, char *argv[]) -> int {
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Welcome to ClockTower!");

    if (const auto sys = GetSystem<base::ConfigSystem>(); sys != nullptr) {
        sys->SetYAMLPath("../../config");
    }

    GetWorld().Init();
    GetWorld().Run();

    return 0;
}