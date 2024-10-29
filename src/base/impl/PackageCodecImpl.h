#pragma once

#include "../IPackageCodec.h"


class UPackageCodecImpl final : public IPackageCodec {
public:
    awaitable<void> Encode(ATcpSocket &socket, IPackage *pkg) override;
    awaitable<void> Decode(ATcpSocket &socket, IPackage *pkg) override;
};
