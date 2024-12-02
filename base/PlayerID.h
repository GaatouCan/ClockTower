#pragma once

#include <cstdint>

static constexpr unsigned int kCrossServerIDOffset = 1'000'000;

struct FPlayerID {
    // 1000 - 99999
    uint32_t localID;
    uint32_t crossID;

    uint32_t GetLocalID() const {
        return localID;
    }

    uint32_t GetCrossID() const {
        return crossID;
    }

    uint64_t ToUInt64() const {
        return crossID * kCrossServerIDOffset + localID;
    }

    FPlayerID &FromUInt64(const uint64_t id) {
        localID = id % kCrossServerIDOffset;
        crossID = id / kCrossServerIDOffset;
        return *this;
    }

    bool operator==(const FPlayerID& other) const {
        return localID == other.localID && crossID == other.crossID;
    }

    bool operator!=(const FPlayerID& other) const {
        return !(*this == other);
    }

    bool IsValid() const {
        return (localID >= 1000 && localID <= 99999) && crossID > 0;
    }
};
