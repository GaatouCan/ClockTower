#pragma once

#include <utility>
#include <vector>
#include <concepts>
#include <nlohmann/json.hpp>

#include "../../common.h"

class ILogicConfig {
public:
    ILogicConfig() = delete;

    explicit ILogicConfig(std::vector<nlohmann::json> configList)
        : mConfigList(std::move(configList)) {}

    virtual ~ILogicConfig() = default;

    DISABLE_COPY_MOVE(ILogicConfig)

    virtual void OnReload(std::vector<nlohmann::json> configList) {
        mConfigList = std::move(configList);
    }

    virtual void Init() {}

protected:
    std::vector<nlohmann::json> mConfigList;
};

template<typename T>
concept LOGIC_CONFIG_TYPE = std::derived_from<T, ILogicConfig>;
