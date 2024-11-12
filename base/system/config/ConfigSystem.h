#pragma once

#include "../../SubSystem.h"
#include "../../GameWorld.h"
#include "ConfigLoader.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>


constexpr auto kServerConfigFile = "/server.yaml";
constexpr auto kServerConfigJSON = "/json";

class UConfigSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(ConfigSystem)

    ~UConfigSystem() override;

    void Init() override;

public:
    void SetYAMLPath(const std::string &path);
    void SetJSONPath(const std::string &path);

    const YAML::Node &GetConfig() const;

    std::optional<nlohmann::json> Find(const std::string &path, uint64_t id) const;

private:
    std::string mYAMLPath;
    std::string mJSONPath;

    YAML::Node mConfig;
    std::unordered_map<std::string, nlohmann::json> mConfigMap;
    std::unordered_map<std::type_index, IConfigLoader *> mLoaderMap;
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
