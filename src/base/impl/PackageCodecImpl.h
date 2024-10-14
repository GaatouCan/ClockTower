#pragma once

#include "../IPackageCodec.h"
#include "Package.h"

namespace base {
    class PackageCodecImpl final : public TPackageCodec<Package>{
    public:
        awaitable<void> encodeT(TcpSocket &socket, Package *pkg) override;
        awaitable<void> decodeT(TcpSocket &socket, Package *pkg) override;
    };
} // base
