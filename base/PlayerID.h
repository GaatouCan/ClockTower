#pragma once

#include <cstdint>

static constexpr unsigned int kCrossServerIDOffset = 1'000'000;
static constexpr unsigned int kPlayerLocalIDBegin = 1'000;
static constexpr unsigned int kPlayerLocalIDEnd = 99'999;

struct FPlayerID {
    // 1000 - 99999
    uint32_t localID;
    uint32_t crossID;

    [[nodiscard]] uint32_t GetLocalID() const {
        return localID;
    }

    [[nodiscard]] uint32_t GetCrossID() const {
        return crossID;
    }

    [[nodiscard]] uint64_t ToUInt64() const {
        return crossID * kCrossServerIDOffset + localID;
    }

    FPlayerID &FromUInt64(const uint64_t id) {
        localID = id % kCrossServerIDOffset;
        crossID = id / kCrossServerIDOffset;
        return *this;
    }

    bool operator<(const FPlayerID &other) const {
        return ToUInt64() < other.ToUInt64();
    }

    bool operator==(const FPlayerID& other) const {
        return localID == other.localID && crossID == other.crossID;
    }

    bool operator!=(const FPlayerID& other) const {
        return !(*this == other);
    }

    [[nodiscard]] bool IsValid() const {
        return (localID >= kPlayerLocalIDBegin && localID <= kPlayerLocalIDEnd) && crossID > 0;
    }
};
