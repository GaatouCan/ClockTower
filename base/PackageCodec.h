#pragma once

#include "common.h"
#include "Package.h"


class IPackageCodec : public UObject {
public:
    virtual awaitable<void> Encode(ATcpSocket &socket, IPackage *pkg) = 0;
    virtual awaitable<void> Decode(ATcpSocket &socket, IPackage *pkg) = 0;
};