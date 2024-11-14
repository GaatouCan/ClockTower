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

inline std::vector<std::function<IConfigLoader*(class UConfigSystem *)>> gConfigLoaderVector;


class UConfigSystem final : public ISubSystem {

    std::string mYAMLPath;
    std::string mJSONPath;

    YAML::Node mConfig;
    std::unordered_map<std::string, nlohmann::json> mConfigMap;
    std::unordered_map<std::type_index, IConfigLoader *> mLoaderMap;

public:
    ~UConfigSystem() override;

    void Init() override;
    [[nodiscard]] constexpr const char *GetSystemName() const override { return "UConfigSystem"; }

    void SetYAMLPath(const std::string &path);
    void SetJSONPath(const std::string &path);

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

    template<typename T>
    requires std::derived_from<T, IConfigLoader>
    class TConfigLoaderRegister {
    public:
        explicit TConfigLoaderRegister(const std::string &path) {
            gConfigLoaderVector.emplace_back([path](UConfigSystem *sys) {
                return sys->AddConfigLoader<T>(path);
            });
        }
    };
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

#define REGISTER_CONFIG_LOADER(loader, ...) \
    static UConfigSystem::TConfigLoaderRegister<loader> g_ConfigLoader_##loader##_Register({__VA_ARGS__});