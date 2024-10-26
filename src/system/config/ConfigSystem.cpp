#include "ConfigSystem.h"
#include "../../common/utils.h"

#include <fstream>
#include <spdlog/spdlog.h>

namespace base {
    void ConfigSystem::Init() {
        spdlog::info("Using configuration file: {}", YAMLPath_ + SERVER_CONFIG_FILE);

        config_ = YAML::LoadFile(YAMLPath_ + SERVER_CONFIG_FILE);

        assert(!config_.IsNull());
        spdlog::info("Checking configuration file");

        assert(!config_["server"].IsNull());
        assert(!config_["server"]["port"].IsNull());
        assert(!config_["server"]["work_thread"].IsNull());

        const std::string jsonPath = !JSONPath_.empty() ? JSONPath_ : YAMLPath_ + SERVER_CONFIG_JSON;

        TraverseFolder(jsonPath, [this, jsonPath](const std::filesystem::directory_entry &entry) {
            if (entry.path().extension().string() == ".json") {
                std::ifstream fs(entry.path());

                auto filepath = entry.path().string();
                filepath = filepath.substr(
                        strlen(jsonPath.c_str()) + 1,
                        filepath.length() - 6 - strlen(jsonPath.c_str()));

                filepath = StringReplace(filepath, '\\', '.');

                configMap_[filepath] = nlohmann::json::parse(fs);
                spdlog::info("\tloaded {}", filepath);
            }
        });
    }

    void ConfigSystem::SetYAMLPath(const std::string &path) {
        YAMLPath_ = path;
    }

    void ConfigSystem::SetJSONPath(const std::string &path) {
        JSONPath_ = path;
    }

    const YAML::Node & ConfigSystem::GetConfig() const {
        return config_;
    }

    std::optional<nlohmann::json> ConfigSystem::Find(const std::string &path, const uint64_t id) const {
        if (const auto it = configMap_.find(path); it != configMap_.end()) {
            if (it->second.contains(std::to_string(id)))
                return it->second[std::to_string(id)];
        }

        return std::nullopt;
    }
} // base