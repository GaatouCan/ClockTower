#pragma once

#include "common.h"
#include "Package.h"

#include <spdlog/spdlog.h>


class IPackageCodec {
public:
    virtual ~IPackageCodec() = default;
    virtual awaitable<void> Encode(ATcpSocket &socket, IPackage *pkg) = 0;
    virtual awaitable<void> Decode(ATcpSocket &socket, IPackage *pkg) = 0;
};

template<PACKAGE_TYPE T>
class TPackageCodec : public IPackageCodec {
public:
    ~TPackageCodec() override = default;

    awaitable<void> Encode(ATcpSocket &socket, IPackage *pkg) override {
        try {
            co_await EncodeT(socket, dynamic_cast<T *>(pkg));
        } catch (std::bad_cast &e) {
            pkg->Invalid();
            spdlog::error("{} - {}", __FUNCTION__, e.what());
        }
    }
    awaitable<void> Decode(ATcpSocket &socket, IPackage *pkg) override {
        try {
            co_await DecodeT(socket, dynamic_cast<T *>(pkg));
        } catch (std::bad_cast &e) {
            pkg->Invalid();
            spdlog::error("{} - {}", __FUNCTION__, e.what());
        }
    }

    virtual awaitable<void> EncodeT(ATcpSocket &socket, T *pkg) = 0;
    virtual awaitable<void> DecodeT(ATcpSocket &socket, T *pkg) = 0;
};