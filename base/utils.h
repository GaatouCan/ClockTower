#pragma once

#include <filesystem>
#include <functional>
#include <thread>

void TraverseFolder(const std::string &folder, const std::function<void(const std::filesystem::directory_entry &)> &func);

std::string StringReplace(std::string source, char toReplace, char replacement);

uint64_t ThreadIdToInt(std::thread::id threadID);

std::string PascalToUnderline(const std::string &src);

long long UnixTime();

uint64_t SetBit(uint64_t, uint32_t);

uint64_t ClearBit(uint64_t, uint32_t);

uint64_t ToggleBit(uint64_t, uint32_t);

bool CheckBit(uint64_t, uint32_t);

std::vector<std::string> SplitString(const std::string &src, char delimiter);

std::vector<int> SplitStringToInt(const std::string &src, char delimiter);