#pragma once

#include "../IPackageCodec.h"


namespace base {
    class UPackageCodecImpl final : public IPackageCodec {
    public:
        awaitable<void> Encode(TcpSocket &socket, IPackage *pkg) override;
        awaitable<void> Decode(TcpSocket &socket, IPackage *pkg) override;
    };
} // base
