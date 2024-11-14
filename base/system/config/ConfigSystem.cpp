#include "ConfigSystem.h"
#include "../../utils.h"

#include <fstream>
#include <spdlog/spdlog.h>

UConfigSystem::UConfigSystem()
    : mLogicConfigLoader(nullptr) {
}

UConfigSystem::~UConfigSystem() {
    for (const auto &loader: std::views::values(mLogicConfigMap)) {
        delete loader;
    }
}

void UConfigSystem::Init() {
    spdlog::info("Using Server Configuration File: {}.", mYAMLPath + kServerConfigFile);

    mConfig = YAML::LoadFile(mYAMLPath + kServerConfigFile);

    assert(!mConfig.IsNull());
    spdlog::info("Checking Server Configuration File.");

    assert(!mConfig["server"].IsNull());
    assert(!mConfig["server"]["port"].IsNull());
    assert(!mConfig["server"]["work_thread"].IsNull());

    assert(!mConfig["log_dir"].IsNull());

    assert(!mConfig["package"].IsNull());
    assert(!mConfig["package"]["magic"].IsNull());
    assert(!mConfig["package"]["version"].IsNull());
    assert(!mConfig["package"]["method"].IsNull());

    assert(!mConfig["package"]["pool"].IsNull());
    assert(!mConfig["package"]["pool"]["default_capacity"].IsNull());
    assert(!mConfig["package"]["pool"]["minimum_capacity"].IsNull());
    assert(!mConfig["package"]["pool"]["expanse_rate"].IsNull());
    assert(!mConfig["package"]["pool"]["expanse_scale"].IsNull());
    assert(!mConfig["package"]["pool"]["collect_rate"].IsNull());
    assert(!mConfig["package"]["pool"]["collect_scale"].IsNull());

    spdlog::info("Server Configuration File Check Successfully.");

    const std::string jsonPath = !mJSONPath.empty() ? mJSONPath : mYAMLPath + kServerConfigJSON;

    TraverseFolder(jsonPath, [this, jsonPath](const std::filesystem::directory_entry &entry) {
        if (entry.path().extension().string() == ".json") {
            std::ifstream fs(entry.path());

            auto filepath = entry.path().string();
            filepath = filepath.substr(
                strlen(jsonPath.c_str()) + 1,
                filepath.length() - 6 - strlen(jsonPath.c_str()));

            filepath = StringReplace(filepath, '\\', '.');

            mJSONConfigMap[filepath] = nlohmann::json::parse(fs);
            spdlog::info("\tLoaded {}.", filepath);
        }
    });

    if (mLogicConfigLoader) {
        std::invoke(mLogicConfigLoader, this);
    }
}

void UConfigSystem::SetYAMLPath(const std::string &path) {
    mYAMLPath = path;
}

void UConfigSystem::SetJSONPath(const std::string &path) {
    mJSONPath = path;
}

void UConfigSystem::SetLogicConfigLoader(const std::function<void(UConfigSystem *)> &loader) {
    mLogicConfigLoader = loader;
}

const YAML::Node &UConfigSystem::GetConfig() const {
    return mConfig;
}

std::optional<nlohmann::json> UConfigSystem::FindConfig(const std::string &path, const uint64_t id) const {
    if (const auto it = mJSONConfigMap.find(path); it != mJSONConfigMap.end()) {
        if (it->second.contains(std::to_string(id)))
            return it->second[std::to_string(id)];
    }

    return std::nullopt;
}
