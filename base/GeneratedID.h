#pragma once

#include <string>

struct FGeneratedID {

    long long time;
    uint64_t random;

    [[nodiscard]] std::string ToString() const;

    static FGeneratedID RandGenerate();

    bool operator==(const FGeneratedID& other) const;
    bool operator!=(const FGeneratedID& other) const;
};
