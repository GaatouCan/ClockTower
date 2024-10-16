#pragma once

#include "../common/common.h"
#include "IPackage.h"

#include <format>

namespace base {
    class IPackageCodec {
    public:
        virtual ~IPackageCodec() = default;

        virtual awaitable<void> Encode(TcpSocket &socket, IPackage *pkg) = 0;
        virtual awaitable<void> Decode(TcpSocket &socket, IPackage *pkg) = 0;
    };

    template<PACKAGE_TYPE T>
    class TPackageCodec : public IPackageCodec {
    public:
        awaitable<void> Encode(TcpSocket &socket, IPackage *pkg) override {
            if (pkg == nullptr)
                throw std::invalid_argument(std::format("{} - Package is nullptr", __func__));
            co_await EncodeT(socket, dynamic_cast<T *>(pkg));
        }

        awaitable<void> Decode(TcpSocket &socket, IPackage *pkg) override {
            if (pkg == nullptr)
                throw std::invalid_argument(std::format("{} - Package is nullptr", __func__));
            co_await DecodeT(socket, dynamic_cast<T *>(pkg));
        }

        virtual awaitable<void> EncodeT(TcpSocket &socket, T *pkg) = 0;
        virtual awaitable<void> DecodeT(TcpSocket &socket, T *pkg) = 0;
    };
}
