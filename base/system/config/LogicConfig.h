#pragma once

#include <vector>
#include <concepts>
#include <nlohmann/json.hpp>

class ILogicConfig {

public:
    ILogicConfig() = delete;

    explicit ILogicConfig(std::vector<nlohmann::json> configList);
    virtual ~ILogicConfig() = default;
};

template<typename T>
concept LOGIC_CONFIG_TYPE = std::derived_from<T, ILogicConfig>;
