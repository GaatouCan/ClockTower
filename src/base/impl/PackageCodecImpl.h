#pragma once

#include "../IPackageCodec.h"
#include "Package.h"

namespace base {
    class PackageCodecImpl final : public TPackageCodec<Package>{
    public:
        awaitable<void> EncodeT(TcpSocket &socket, Package *pkg) override;
        awaitable<void> DecodeT(TcpSocket &socket, Package *pkg) override;
    };
} // base
