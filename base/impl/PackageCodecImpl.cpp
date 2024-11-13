#include "PackageCodecImpl.h"
#include "Package.h"


awaitable<void> UPackageCodecImpl::EncodeT(ATcpSocket &socket, FPackage *pkg) {
    try {
        if (const auto len = co_await async_write(socket, asio::buffer(&pkg->mHeader, FPackage::headerSize)); len == 0) {
            spdlog::warn("{} - Write package header length equal zero", __func__);
            pkg->Invalid();
            co_return;
        }

        if (pkg->mHeader.size == 0)
            co_return;

        co_await async_write(socket, asio::buffer(pkg->mData));
    } catch (std::exception &e) {
        spdlog::warn("{} - {}", __func__, e.what());
    }
    co_return;
}

awaitable<void> UPackageCodecImpl::DecodeT(ATcpSocket &socket, FPackage *pkg) {
    try {
        if (const auto len = co_await async_read(socket, asio::buffer(&pkg->mHeader, FPackage::headerSize)); len == 0) {
            spdlog::warn("{} - Read package header length equal zero", __func__);
            pkg->Invalid();
            co_return;
        }

        if (pkg->mHeader.size == 0)
            co_return;

        pkg->mData.resize(pkg->mHeader.size);
        co_await async_read(socket, asio::buffer(pkg->mData));
    } catch (std::exception &e) {
        spdlog::warn("{} - {}", __func__, e.what());
    }
    co_return;
}
