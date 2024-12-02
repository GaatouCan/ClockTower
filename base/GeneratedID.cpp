#include "GeneratedID.h"
#include "utils.h"

#include <spdlog/fmt/fmt.h>
#include <absl/random/random.h>

std::string FGeneratedID::ToString() const {
    return fmt::format("{}-{}", time, random);
}

FGeneratedID & FGeneratedID::FromString(const std::string &str) {
    const std::vector<std::string> res = SplitString(str, '-');
    time = std::stoll(res[0]);
    random = std::stoull(res[1]);
    return *this;
}

FGeneratedID FGeneratedID::RandGenerate() {
    static absl::BitGen gen;
    const uint64_t number = absl::Uniform(gen, 100000, 999999);
    return {
        UnixTime(),
        number
    };
}

bool FGeneratedID::operator<(const FGeneratedID &other) const {
    if (time < other.time) {
        return true;
    }
    if (time == other.time) {
        return random < other.random;
    }
    return false;
}

bool FGeneratedID::operator==(const FGeneratedID &other) const {
    return time == other.time && random == other.random;
}

bool FGeneratedID::operator!=(const FGeneratedID &other) const {
    return !(*this == other);
}
