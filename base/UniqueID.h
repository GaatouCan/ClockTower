#pragma once

#include "utils.h"

struct FUniqueID {

    long long time;
    uint64_t random;

    std::string ToString() const;

    static FUniqueID RandGenerate();

    bool operator==(const FUniqueID& other) const;
    bool operator!=(const FUniqueID& other) const;
};
