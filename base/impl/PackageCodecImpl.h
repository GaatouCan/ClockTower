#pragma once

#include "../PackageCodec.h"
#include "Package.h"

class UPackageCodecImpl final : public TPackageCodec<FPackage> {
public:
    awaitable<void> EncodeT(ATcpSocket &socket, FPackage *pkg) override;
    awaitable<void> DecodeT(ATcpSocket &socket, FPackage *pkg) override;
};
