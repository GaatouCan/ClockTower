#pragma once

#include "../../base/ISubSystem.h"
#include "../../base/GameWorld.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>


constexpr auto SERVER_CONFIG_FILE = "/server.yaml";
constexpr auto SERVER_CONFIG_JSON = "/json";

class UConfigSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(ConfigSystem)
    void Init() override;

public:
    void SetYAMLPath(const std::string &path);
    void SetJSONPath(const std::string &path);

    const YAML::Node &GetConfig() const;

    std::optional<nlohmann::json> Find(const std::string &path, uint64_t id) const;

private:
    std::string YAMLPath_;
    std::string JSONPath_;

    YAML::Node config_;
    std::unordered_map<std::string, nlohmann::json> configMap_;
};

inline const YAML::Node &GetServerConfig() {
    const auto cfgSys = GetSystem<UConfigSystem>();
    if (cfgSys == nullptr) {
        spdlog::critical("{} - Fail to found ConfigSystem", __func__);
        GetWorld().Shutdown();
        exit(-1);
    }

    return cfgSys->GetConfig();
}
