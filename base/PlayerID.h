#pragma once

#include <cstdint>

static constexpr unsigned int kCrossServerIDOffset = 1'000'000'000;

struct FPlayerID {
    uint32_t localID;
    uint32_t crossID;

    uint32_t GetLocalID() const {
        return localID;
    }

    uint32_t GetCrossID() const {
        return crossID;
    }

    uint64_t GetPlayerID() const {
        return crossID * kCrossServerIDOffset + localID;
    }
};
