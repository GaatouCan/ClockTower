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

unsigned int GetDayOfMonth(const std::chrono::time_point<std::chrono::system_clock> point) {
    const auto today = std::chrono::floor<std::chrono::days>(point);
    const auto ymd = std::chrono::year_month_day{today};
    return unsigned int{ymd.day()};
}

unsigned int GetDayOfYear(const std::chrono::time_point<std::chrono::system_clock> point) {
    const auto today = std::chrono::floor<std::chrono::days>(point);
    const auto ymd = std::chrono::year_month_day{today};
    const auto current_year = ymd.year();
    const auto first_day_of_year = std::chrono::year_month_day{current_year, std::chrono::January, std::chrono::day(1)};

    const auto days_since_start_of_year = std::chrono::sys_days(ymd) - std::chrono::sys_days(first_day_of_year);

    return days_since_start_of_year.count() + 1;
}

unsigned int GetPassedDays(const std::chrono::time_point<std::chrono::system_clock> pointX, const std::chrono::time_point<std::chrono::system_clock> pointY) {
    if (pointY <= pointX)
        return 0;

    const auto daysX = std::chrono::floor<std::chrono::days>(pointX);
    const auto daysY = std::chrono::floor<std::chrono::days>(pointY);

    const auto diff = daysY - daysX;
    return diff.count() + 1;
}


