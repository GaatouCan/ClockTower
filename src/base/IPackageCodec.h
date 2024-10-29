#pragma once

#include "../common/common.h"
#include "IPackage.h"

namespace base {
    class IPackageCodec {
    public:
        virtual ~IPackageCodec() = default;

        virtual awaitable<void> Encode(TcpSocket &socket, IPackage *pkg) = 0;
        virtual awaitable<void> Decode(TcpSocket &socket, IPackage *pkg) = 0;
    };
}
