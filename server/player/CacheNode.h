#pragma once

#include <cstdint>
#include <common.h>

struct FCacheNode {
    uint64_t pid;
    ATimePoint lastLoginTime;
    ATimePoint lastLogoutTime;

    [[nodiscard]] bool IsOnline() const {
        constexpr auto zeroPoint = ATimePoint();
        const auto now = NowTimePoint();

        return (lastLoginTime > zeroPoint && lastLoginTime <= now)
               && (lastLogoutTime > zeroPoint && lastLogoutTime <= now)
               && lastLoginTime > lastLogoutTime;
    }
};
