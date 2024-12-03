#include "utils.h"

#include <cassert>
#include <ctime>
#include <spdlog/fmt/bundled/chrono.h>


void TraverseFolder(const std::string &folder, const std::function<void(const std::filesystem::directory_entry &)> &func) {
    for (const auto &entry: std::filesystem::directory_iterator(folder)) {
        if (entry.is_directory()) {
            TraverseFolder(entry.path().string(), func);
        } else {
            std::invoke(func, entry);
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

long long ToUnixTime(const std::chrono::time_point<std::chrono::system_clock> point) {
    const auto durationSinceEpoch = point.time_since_epoch();
    const auto secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);
    return secondsSinceEpoch.count();
}

uint64_t SetBit(const uint64_t number, const uint32_t n) {
    assert(n < sizeof(uint64_t) * 8);
    return number | static_cast<uint64_t>(1) << n;
}

uint64_t ClearBit(const uint64_t number, const uint32_t n) {
    assert(n < sizeof(uint64_t) * 8);
    return number & ~(static_cast<uint64_t>(1) << n);
}

uint64_t ToggleBit(const uint64_t number, const uint32_t n) {
    assert(n < sizeof(uint64_t) * 8);
    return number ^ static_cast<uint64_t>(1) << n;
}

bool CheckBit(const uint64_t number, const uint32_t n) {
    assert(n < sizeof(uint64_t) * 8);
    return number & static_cast<uint64_t>(1) << n;
}

std::vector<std::string> SplitString(const std::string &src, const char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(src);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}


std::vector<int> SplitStringToInt(const std::string &src, const char delimiter) {
    std::vector<int> result;
    std::stringstream ss(src);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(std::stoi(item));}

    return result;
}

int GetDayOfWeek(const std::chrono::time_point<std::chrono::system_clock> point) {
    const std::time_t current_time = std::chrono::system_clock::to_time_t(point);
    std::tm tm{};
#ifdef WIN32
    localtime_s(&tm, &current_time);
#else
    localtime_r(&current_time, &tm);
#endif
    return tm.tm_wday;
}

unsigned GetDayOfMonth(const std::chrono::time_point<std::chrono::system_clock> point) {
    const auto local_time = std::chrono::current_zone()->to_local(point);
    const std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(local_time));
    return static_cast<unsigned>(ymd.day());
}

int GetDayOfYear(const std::chrono::time_point<std::chrono::system_clock> point) {
    const auto local_time = std::chrono::current_zone()->to_local(point);

    const auto today = std::chrono::floor<std::chrono::days>(local_time);

    const auto year = std::chrono::floor<std::chrono::years>(local_time);
    const auto first_day_of_year = std::chrono::floor<std::chrono::days>(year);

    return today.time_since_epoch().count() - first_day_of_year.time_since_epoch().count() + 1;
}

unsigned int GetPassedDays(const std::chrono::time_point<std::chrono::system_clock> pointX, const std::chrono::time_point<std::chrono::system_clock> pointY) {
    if (pointY <= pointX)
        return 0;

    const auto local_time_x = std::chrono::current_zone()->to_local(pointX);
    const auto local_time_y = std::chrono::current_zone()->to_local(pointY);

    const auto days_x = std::chrono::floor<std::chrono::days>(local_time_x);
    const auto days_y = std::chrono::floor<std::chrono::days>(local_time_y);

    return days_y.time_since_epoch().count() - days_x.time_since_epoch().count();
}

std::chrono::time_point<std::chrono::system_clock> GetDayZeroTime(const std::chrono::time_point<std::chrono::system_clock> point) {
    const auto days =  std::chrono::floor<std::chrono::days>(std::chrono::current_zone()->to_local(point));
    return std::chrono::current_zone()->to_sys(days);
}
