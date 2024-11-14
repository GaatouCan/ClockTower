#pragma once

#include "../../SubSystem.h"
#include "../../GameWorld.h"
#include "LogicConfig.h"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>


constexpr auto kServerConfigFile = "/server.yaml";
constexpr auto kServerConfigJSON = "/json";


class UConfigSystem final : public ISubSystem {

    std::string mYAMLPath;
    std::string mJSONPath;

    YAML::Node mConfig;
    std::unordered_map<std::string, nlohmann::json> mJSONConfigMap;
    std::unordered_map<std::type_index, ILogicConfig *> mLogicConfigMap;

    std::function<void(UConfigSystem *)> mLogicConfigLoader;

public:
    UConfigSystem();
    ~UConfigSystem() override;

    void Init() override;
    [[nodiscard]] constexpr const char *GetSystemName() const override { return "UConfigSystem"; }

    void SetYAMLPath(const std::string &path);
    void SetJSONPath(const std::string &path);

    void SetLogicConfigLoader(const std::function<void(UConfigSystem *)> &loader);

    template<LOGIC_CONFIG_TYPE T>
    void CreateLogicConfig(const std::vector<std::string> &pathList) {
        std::vector<nlohmann::json> configs;
        for (const auto &path : pathList) {
            if (const auto iter = mJSONConfigMap.find(path); iter != mJSONConfigMap.end()) {
                configs.push_back(iter->second);
            }
        }
        mLogicConfigMap.insert_or_assign(typeid(T), new T(configs));
    }

    template<LOGIC_CONFIG_TYPE T>
    T *FindLogicConfig() {
        if (const auto iter = mLogicConfigMap.find(typeid(T)); iter != mLogicConfigMap.end()) {
            return dynamic_cast<T *>(iter->second);
        }
        return nullptr;
    }

    const YAML::Node &GetConfig() const;

    std::optional<nlohmann::json> FindConfig(const std::string &path, uint64_t id) const;
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