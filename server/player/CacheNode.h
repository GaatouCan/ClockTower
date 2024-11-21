#pragma once

#include <cstdint>

struct FCacheNode {
    uint64_t pid;
    ATimePoint lastLoginTime;
    ATimePoint lastLogoutTime;

    bool IsOnline() const {
        constexpr auto zeroPoint = ATimePoint();
        const auto now = std::chrono::steady_clock::now();

        return (lastLoginTime > zeroPoint && lastLoginTime <= now)
               && (lastLogoutTime > zeroPoint && lastLogoutTime <= now)
               && lastLoginTime > lastLogoutTime;
    }
};
