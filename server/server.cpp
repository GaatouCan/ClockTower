#include <GameWorld.h>

#include <system/protocol/ProtocolSystem.h>
#include <system/login/LoginSystem.h>

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

    // 设置服务器配置文件路径
    if (const auto sys = GetSystem<UConfigSystem>(); sys != nullptr) {
#ifdef WIN32
        sys->SetYAMLPath("../../config");
#else
        sys->SetYAMLPath("../config");
#endif
        sys->SetLogicConfigLoader(&LoadLogicConfig);
        sys->SetLoggerLoader(&InitLogger);
    }

    // 注册程式生成协议定义以及协议处理器
    if (const auto sys = GetSystem<UProtocolSystem>(); sys != nullptr) {
        LoadProtocol(sys);
        sys->SetHandler<UProtocolHandlerImpl>();
    }

    // 设置玩家登陆请求处理器
    if (const auto sys = GetSystem<ULoginSystem>(); sys != nullptr) {
        sys->SetHandler<ULoginHandlerImpl>();
    }

    // 注册Manager
    if (const auto sys = GetSystem<UManagerSystem>(); sys != nullptr) {
        sys->SetManagerLoader(&LoadManager);
    }

    // 设置新连接处理
    GetWorld().FilterConnection([](const AConnectionPointer &conn) {
        if (conn != nullptr) {
            conn->SetHandler<UConnectionHandlerImpl>();
        }
    });

    // 启动服务器
    GetWorld().Init();
    GetWorld().Run();

    return 0;
}