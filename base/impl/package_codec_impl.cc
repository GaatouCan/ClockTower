#include "package_codec_impl.h"
#include "package.h"
#include "../connection.h"

awaitable<void> UPackageCodecImpl::EncodeT(FPackage *pkg) {
    if (const auto len = co_await async_write(mConn.lock()->GetSocket(), asio::buffer(&pkg->mHeader, FPackage::headerSize)); len == 0) {
        spdlog::warn("{} - Write package header length equal zero", __FUNCTION__);
        pkg->Invalid();
        co_return;
    }

    if (pkg->mHeader.mLength == 0)
        co_return;

    co_await async_write(mConn.lock()->GetSocket(), asio::buffer(pkg->mBytes));
}

awaitable<void> UPackageCodecImpl::DecodeT(FPackage *pkg) {
    if (const auto len = co_await async_read(mConn.lock()->GetSocket(), asio::buffer(&pkg->mHeader, FPackage::headerSize)); len == 0) {
        spdlog::warn("{} - Read package header length equal zero", __FUNCTION__);
        pkg->Invalid();
        co_return;
    }

    if (pkg->mHeader.mLength == 0)
        co_return;

    pkg->mBytes.resize(pkg->mHeader.mLength);
    co_await async_read(mConn.lock()->GetSocket(), asio::buffer(pkg->mBytes));
}
