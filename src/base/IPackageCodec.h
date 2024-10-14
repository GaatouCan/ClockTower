#pragma once

#include "../common/common.h"
#include "IPackage.h"

#include <format>

namespace base {
    class IPackageCodec {
    public:
        virtual ~IPackageCodec() = default;

        virtual awaitable<void> encode(TcpSocket &socket, IPackage *pkg) = 0;
        virtual awaitable<void> decode(TcpSocket &socket, IPackage *pkg) = 0;
    };

    template<PACKAGE_TYPE T>
    class TPackageCodec : public IPackageCodec {
    public:
        awaitable<void> encode(TcpSocket &socket, IPackage *pkg) override {
            if (pkg == nullptr)
                throw std::invalid_argument(std::format("{} - Package is nullptr", __func__));
            co_await encodeT(socket, dynamic_cast<T *>(pkg));
        }

        awaitable<void> decode(TcpSocket &socket, IPackage *pkg) override {
            if (pkg == nullptr)
                throw std::invalid_argument(std::format("{} - Package is nullptr", __func__));
            co_await decodeT(socket, dynamic_cast<T *>(pkg));
        }

        virtual awaitable<void> encodeT(TcpSocket &socket, T *pkg) = 0;
        virtual awaitable<void> decodeT(TcpSocket &socket, T *pkg) = 0;
    };
}
