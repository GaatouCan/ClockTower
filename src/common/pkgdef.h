#pragma once

#include "../base/impl/Package.h"
#include "../system/config/ConfigSystem.h"

inline IPackage *CreatePackage() {
    return new FPackage();
}

inline void InitPackage(IPackage *pkg) {
    const auto tmp = dynamic_cast<FPackage *>(pkg);
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
            tmp->ChangeMethod(ECodecMethod::LINE_BASED);
        else if (method == "Protobuf")
            tmp->ChangeMethod(ECodecMethod::PROTOBUF);
    }
}
