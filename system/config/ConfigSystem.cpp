#include "ConfigSystem.h"
#include "../../common/utils.h"

#include <fstream>
#include <spdlog/spdlog.h>


void UConfigSystem::Init() {
    spdlog::info("Using configuration file: {}.", mYAMLPath + SERVER_CONFIG_FILE);

    mConfig = YAML::LoadFile(mYAMLPath + SERVER_CONFIG_FILE);

    assert(!mConfig.IsNull());
    spdlog::info("Checking configuration file.");

    assert(!mConfig["server"].IsNull());
    assert(!mConfig["server"]["port"].IsNull());
    assert(!mConfig["server"]["work_thread"].IsNull());

    const std::string jsonPath = !mJSONPath.empty() ? mJSONPath : mYAMLPath + SERVER_CONFIG_JSON;

    TraverseFolder(jsonPath, [this, jsonPath](const std::filesystem::directory_entry &entry) {
        if (entry.path().extension().string() == ".json") {
            std::ifstream fs(entry.path());

            auto filepath = entry.path().string();
            filepath = filepath.substr(
                strlen(jsonPath.c_str()) + 1,
                filepath.length() - 6 - strlen(jsonPath.c_str()));

            filepath = StringReplace(filepath, '\\', '.');

            mConfigMap[filepath] = nlohmann::json::parse(fs);
            spdlog::info("\tloaded {}.", filepath);
        }
    });
}

void UConfigSystem::SetYAMLPath(const std::string &path) {
    mYAMLPath = path;
}

void UConfigSystem::SetJSONPath(const std::string &path) {
    mJSONPath = path;
}

const YAML::Node &UConfigSystem::GetConfig() const {
    return mConfig;
}

std::optional<nlohmann::json> UConfigSystem::Find(const std::string &path, const uint64_t id) const {
    if (const auto it = mConfigMap.find(path); it != mConfigMap.end()) {
        if (it->second.contains(std::to_string(id)))
            return it->second[std::to_string(id)];
    }

    return std::nullopt;
}
