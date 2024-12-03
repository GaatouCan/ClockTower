#pragma once

#include <filesystem>
#include <functional>
#include <thread>

void TraverseFolder(const std::string &folder, const std::function<void(const std::filesystem::directory_entry &)> &func);

std::string StringReplace(std::string source, char toReplace, char replacement);

uint64_t ThreadIdToInt(std::thread::id threadID);

std::string PascalToUnderline(const std::string &src);

long long UnixTime();

long long ToUnixTime(std::chrono::time_point<std::chrono::system_clock> point);

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
int GetDayOfWeek(std::chrono::time_point<std::chrono::system_clock> point = std::chrono::system_clock::now());

unsigned GetDayOfMonth(std::chrono::time_point<std::chrono::system_clock> point = std::chrono::system_clock::now());

int GetDayOfYear(std::chrono::time_point<std::chrono::system_clock> point = std::chrono::system_clock::now());

unsigned int GetPassedDays(std::chrono::time_point<std::chrono::system_clock> pointX, std::chrono::time_point<std::chrono::system_clock> pointY = std::chrono::system_clock::now());

std::chrono::time_point<std::chrono::system_clock> GetDayZeroTime(std::chrono::time_point<std::chrono::system_clock> point = std::chrono::system_clock::now());
