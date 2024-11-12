#pragma once

#include <vector>
#include <nlohmann/json.hpp>

class IConfigLoader {

public:
    IConfigLoader() = delete;

    explicit IConfigLoader(std::vector<nlohmann::json> configList);
    virtual ~IConfigLoader() = default;
};