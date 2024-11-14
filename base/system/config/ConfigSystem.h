#pragma once

#include "../../SubSystem.h"
#include "../../GameWorld.h"
#include "ConfigLoader.h"

#include <concepts>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>


constexpr auto kServerConfigFile = "/server.yaml";
constexpr auto kServerConfigJSON = "/json";

class UConfigSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(ConfigSystem)

    UConfigSystem();
    ~UConfigSystem() override;

    void Init() override;

public:
    void SetYAMLPath(const std::string &path);
    void SetJSONPath(const std::string &path);

    void SetLoaderFunctor(const std::function<void(UConfigSystem *)> &func);

    template<typename T>
    requires std::derived_from<T, IConfigLoader>
    void AddConfigLoader(const std::vector<std::string> &pathList) {
        std::vector<nlohmann::json> configs;
        for (const auto &path : pathList) {
            if (const auto iter = mConfigMap.find(path); iter != mConfigMap.end()) {
                configs.push_back(iter->second);
            }
        }
        mLoaderMap.insert_or_assign(typeid(T), new T(configs));
    }

    template<typename T>
    requires std::derived_from<T, IConfigLoader>
    T *FindLoader() {
        if (const auto iter = mLoaderMap.find(typeid(T)); iter != mLoaderMap.end()) {
            return dynamic_cast<T *>(iter->second);
        }
        return nullptr;
    }

    const YAML::Node &GetConfig() const;

    std::optional<nlohmann::json> Find(const std::string &path, uint64_t id) const;

private:
    std::string mYAMLPath;
    std::string mJSONPath;

    YAML::Node mConfig;
    std::unordered_map<std::string, nlohmann::json> mConfigMap;
    std::unordered_map<std::type_index, IConfigLoader *> mLoaderMap;

    std::function<void(UConfigSystem*)> mLoader;
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
