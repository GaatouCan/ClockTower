#pragma once

#include <string>

#ifdef __linux__
#include <cstdint>
#endif

struct FGeneratedID {

    long long time;
    uint64_t random;

    [[nodiscard]] std::string ToString() const;

    FGeneratedID &FromString(const std::string &str);

    static FGeneratedID RandGenerate();

    bool operator==(const FGeneratedID& other) const;
    bool operator!=(const FGeneratedID& other) const;
};
