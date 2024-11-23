#include "UniqueID.h"

#include <spdlog/fmt/fmt.h>
#include <absl/random/random.h>

std::string FUniqueID::ToString() const {
    return fmt::format("{}-{}", time, random);
}

FUniqueID FUniqueID::RandGenerate() {
    static absl::BitGen gen;
    const uint64_t number = absl::Uniform(gen, 100000, 999999);
    return {
        UnixTime(),
        number
    };
}

bool FUniqueID::operator==(const FUniqueID &other) const {
    return time == other.time && random == other.random;
}

bool FUniqueID::operator!=(const FUniqueID &other) const {
    return !(*this == other);
}
