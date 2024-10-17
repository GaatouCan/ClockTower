#include <spdlog/spdlog.h>
#include "base/GameWorld.h"

auto main(int argc, char *argv[]) -> int {
    spdlog::info("Hello World!");

    GetWorld().Init();
    GetWorld().Run();

    return 0;
}