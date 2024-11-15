#include "AvatarLogicConfig.h"


UAvatarLogicConfig::UAvatarLogicConfig(std::vector<nlohmann::json> configList)
    : ILogicConfig(std::move(configList)) {

    for (const auto &val : mConfigList) {
        // TODO
    }
}

UAvatarLogicConfig::~UAvatarLogicConfig() {
}
