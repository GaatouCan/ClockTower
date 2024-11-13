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
#include "common/logger_def.h"

auto main(int argc, char *argv[]) -> int {
    spdlog::info("Welcome to the ClockTower!");

    InitLogger();

    set_default_logger(spdlog::get("base_logger"));

#ifdef _DEBUG
    { // logger test
        const auto gBaseLogger = spdlog::get("base_logger");
        gBaseLogger->info("Hello World, This is a logger test.");
    }
#endif

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