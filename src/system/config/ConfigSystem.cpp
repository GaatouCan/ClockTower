#include "ConfigSystem.h"

namespace base {
    void ConfigSystem::init() {
    }

    const YAML::Node & ConfigSystem::serverConfig() const {
        return config_;
    }
} // base