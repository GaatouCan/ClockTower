#pragma once

#include "../../base/ISubSystem.h"
#include "../../base/GameWorld.h"

#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>

namespace base {
    class ConfigSystem final : public ISubSystem {

        SUB_SYSTEM_BODY(ConfigSystem)

    public:
        void init() override;

        const YAML::Node &serverConfig() const;

    private:
        YAML::Node config_;
        std::unordered_map<std::string, std::unordered_map<uint64_t, nlohmann::json>> configMap_;
    };
} // base

inline const YAML::Node &GetServerConfig() {
    const auto sys = GetSystem<base::ConfigSystem>();
    if (sys == nullptr)
        throw std::runtime_error("Config System not found");

    return sys->serverConfig();
}