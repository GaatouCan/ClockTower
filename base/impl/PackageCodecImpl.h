#pragma once

#include "../PackageCodec.h"
#include "Package.h"

class UPackageCodecImpl final : public TPackageCodec<FPackage> {
public:
    explicit UPackageCodecImpl(const std::shared_ptr<UConnection> &conn) : TPackageCodec(conn) {}
    awaitable<void> EncodeT(FPackage *pkg) override;
    awaitable<void> DecodeT(FPackage *pkg) override;
};
