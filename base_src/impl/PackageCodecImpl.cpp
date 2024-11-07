#include "PackageCodecImpl.h"
#include "Package.h"

#include <spdlog/spdlog.h>


awaitable<void> UPackageCodecImpl::Encode(ATcpSocket &socket, IPackage *pkg) {
    try {
        const auto tmp = dynamic_cast<FPackage *>(pkg);

        if (const auto len = co_await async_write(socket, asio::buffer(&tmp->mHeader, FPackage::headerSize)); len == 0) {
            spdlog::warn("{} - Write package header length equal zero", __func__);
            tmp->Invalid();
            co_return;
        }

        if (tmp->mHeader.size == 0)
            co_return;

        co_await async_write(socket, asio::buffer(tmp->mData));
    } catch (std::exception &e) {
        spdlog::error("{} - {}", __func__, e.what());
    }
    co_return;
}

awaitable<void> UPackageCodecImpl::Decode(ATcpSocket &socket, IPackage *pkg) {
    try {
        const auto tmp = dynamic_cast<FPackage *>(pkg);

        if (const auto len = co_await async_read(socket, asio::buffer(&tmp->mHeader, FPackage::headerSize)); len == 0) {
            spdlog::warn("{} - Read package header length equal zero", __func__);
            tmp->Invalid();
            co_return;
        }

        if (tmp->mHeader.size == 0)
            co_return;

        tmp->mData.resize(tmp->mHeader.size);
        co_await async_read(socket, asio::buffer(tmp->mData));
    } catch (std::exception &e) {
        spdlog::error("{} - {}", __func__, e.what());
    }
    co_return;
}
