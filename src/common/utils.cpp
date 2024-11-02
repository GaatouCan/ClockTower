#include "utils.h"

#include <assert.h>


void TraverseFolder(const std::string &folder,
                    const std::function<void(const std::filesystem::directory_entry &)> &func) {
    for (const auto &entry: std::filesystem::directory_iterator(folder)) {
        if (entry.is_directory()) {
            TraverseFolder(entry.path().string(), func);
        } else {
            func(entry);
        }
    }
}

std::string StringReplace(std::string source, const char toReplace, const char replacement) {
    size_t pos = 0;
    while ((pos = source.find(toReplace, pos)) != std::string::npos) {
        source.replace(pos, 1, 1, replacement);
        pos += 1;
    }
    return source;
}

uint64_t ThreadIdToInt(const std::thread::id threadID) {
    std::stringstream ss;
    ss << threadID;
    return std::stoull(ss.str());
}

std::string PascalToUnderline(const std::string &src) {
    std::string res;

    const auto length = src.size();
    res.resize(length * 2);

    bool bFirstLetter = true;
    size_t count = 0;

    for (auto idx = 0; idx < length; idx++) {
        if (src[idx] >= 'A' && src[idx] <= 'Z' && bFirstLetter) {
            bFirstLetter = false;
            res[idx] = src[idx] - 'A' + 'a';
        } else if (src[idx] >= 'A' && src[idx] <= 'Z' && !bFirstLetter) {
            res[idx + count] = '_';
            count++;
            res[idx + count] = src[idx] - 'A' + 'a';
        } else {
            res[idx + count] = src[idx];
        }
    }

    return res;
}

long long UnixTime() {
    const auto now = std::chrono::system_clock::now();
    const auto durationSinceEpoch = now.time_since_epoch();
    const auto secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);
    return secondsSinceEpoch.count();
}

uint64_t SetBit(const uint64_t number, const uint32_t n) {
    assert(n < sizeof(uint64_t) * 8);
    return number | (static_cast<uint64_t>(1) << n);
}

uint64_t ClearBit(const uint64_t number, const uint32_t n) {
    assert(n < sizeof(uint64_t) * 8);
    return number & ~(static_cast<uint64_t>(1) << n);
}

uint64_t ToggleBit(const uint64_t number, const uint32_t n) {
    assert(n < sizeof(uint64_t) * 8);
    return number ^ (static_cast<uint64_t>(1) << n);
}

bool CheckBit(const uint64_t number, const uint32_t n) {
    assert(n < sizeof(uint64_t) * 8);
    return number & (static_cast<uint64_t>(1) << n);
}


