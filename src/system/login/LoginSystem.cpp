#include "LoginSystem.h"

namespace base {
    void LoginSystem::init() {
    }

    bool LoginSystem::verifyAddress(const std::shared_ptr<Connection> &conn) {
        return true;
    }

    uint64_t LoginSystem::verifyToken(uint64_t pid, const std::string &token) {
        return pid;
    }
} // base