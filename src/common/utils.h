#pragma once

#include "../base/IPackage.h"

#include <filesystem>
#include <functional>
#include <thread>

base::IPackage *CreatePackage();

void TraverseFolder(const std::string &folder, const std::function<void(const std::filesystem::directory_entry &)> &func);

std::string StringReplace(std::string source, char toReplace, char replacement);

uint64_t ThreadIdToInt(std::thread::id threadID);

std::string PascalToUnderline(const std::string &src);

long long CurrentTimeCount();