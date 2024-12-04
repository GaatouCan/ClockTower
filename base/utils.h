#pragma once

#include <filesystem>
#include <functional>
#include <thread>
#include <chrono>

using ATimePoint = std::chrono::time_point<std::chrono::system_clock>;


void TraverseFolder(const std::string &folder, const std::function<void(const std::filesystem::directory_entry &)> &func);

std::string StringReplace(std::string source, char toReplace, char replacement);

uint64_t ThreadIdToInt(std::thread::id threadID);

std::string PascalToUnderline(const std::string &src);

long long UnixTime();

long long ToUnixTime(ATimePoint point);

uint64_t SetBit(uint64_t, uint32_t);

uint64_t ClearBit(uint64_t, uint32_t);

uint64_t ToggleBit(uint64_t, uint32_t);

bool CheckBit(uint64_t, uint32_t);

std::vector<std::string> SplitString(const std::string &src, char delimiter);

std::vector<int> SplitStringToInt(const std::string &src, char delimiter);

/**
 * Get Day Of The Week
 * @param point Time Point, Default Now
 * @return From 0 To 6, Means Sunday(0) To StaterDay(6)
 */
int GetDayOfWeek(ATimePoint point = std::chrono::system_clock::now());

unsigned GetDayOfMonth(ATimePoint point = std::chrono::system_clock::now());

int GetDayOfYear(ATimePoint point = std::chrono::system_clock::now());

/**
 * 往日不再
 * @param former 较前的时间点
 * @param latter 较后的时间点 默认当前时间点
 * @return 经过的天数 同一天为0
 */
int GetDaysGone(ATimePoint former, ATimePoint latter = std::chrono::system_clock::now());

ATimePoint GetDayZeroTime(ATimePoint point = std::chrono::system_clock::now());
