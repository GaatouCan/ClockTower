#include "../base/GameWorld.h"

#include "../base/system/protocol/ProtocolSystem.h"
#include "../base/system/login/LoginSystem.h"

#include "impl/ConnectionHandlerImpl.h"
#include "impl/ProtocolHandlerImpl.h"
#include "impl/LoginHandlerImpl.h"

#include "common/config_def.h"
#include "common/proto_def.h"
#include "common/logger_def.h"
#include "common/manager_def.h"

auto main(int argc, char *argv[]) -> int {
    spdlog::info("Welcome to the ClockTower!");

    if (const auto sys = GetSystem<UConfigSystem>(); sys != nullptr) {
        sys->SetYAMLPath("../../config");
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