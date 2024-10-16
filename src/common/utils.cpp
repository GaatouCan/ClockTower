#include "utils.h"

#include "../base/impl/Package.h"
#include "../system/config/ConfigSystem.h"

base::IPackage *CreatePackage() {
    return new base::Package();
}

void InitPackage(base::IPackage *pkg) {
    const auto tmp = dynamic_cast<base::Package *>(pkg);
    if (tmp == nullptr)
        return;

    const auto &cfg = GetServerConfig();

    if (cfg["package"].IsNull())
        return;

    if (!cfg["package"]["magic"].IsNull())
        tmp->SetMagic(cfg["package"]["magic"].as<uint32_t>());

    if (!cfg["package"]["version"].IsNull())
        tmp->SetVersion(cfg["package"]["version"].as<uint32_t>());

    if (!cfg["package"]["method"].IsNull()) {
        if (const auto method = cfg["package"]["method"].as<std::string>(); method == "LineBased")
            tmp->ChangeMethod(base::CodecMethod::LINE_BASED);
        else if (method == "Protobuf")
            tmp->ChangeMethod(base::CodecMethod::PROTOBUF);
    }
}

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

long long CurrentTimeCount() {
    const auto now = std::chrono::system_clock::now();
    const auto durationSinceEpoch = now.time_since_epoch();
    const auto secondsSinceEpoch = std::chrono::duration_cast<std::chrono::seconds>(durationSinceEpoch);
    return secondsSinceEpoch.count();
}
