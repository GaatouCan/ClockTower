#include "PackageCodecImpl.h"

#include <spdlog/spdlog.h>

namespace base {
    awaitable<void> PackageCodecImpl::encodeT(TcpSocket &socket, Package *pkg) {
        {
            auto [ec, len] = co_await async_write(socket, asio::buffer(&pkg->header_, Package::headerSize));
            if (ec) {
                spdlog::warn("{} - Write package header failed: {}", __func__, ec.message());
                pkg->invalid();
                co_return;
            }

            if (len == 0) {
                spdlog::warn("{} - Write package header length equal zero", __func__);
                pkg->invalid();
                co_return;
            }
        }

        if (pkg->header_.size == 0)
            co_return;

        if (auto [ec, len] = co_await async_write(socket, asio::buffer(pkg->data_)); ec) {
            spdlog::warn("{} - Write data failed: {}", __func__, ec.message());
            pkg->invalid();
        }
        co_return;
    }

    awaitable<void> PackageCodecImpl::decodeT(TcpSocket &socket, Package *pkg) {
        {
            auto [ec, len] = co_await async_read(socket, asio::buffer(&pkg->header_, Package::headerSize));
            if (ec) {
                spdlog::warn("{} - Read package header failed: {}", __func__, ec.message());
                pkg->invalid();
                co_return;
            }

            if (len == 0) {
                spdlog::warn("{} - Read package header length equal zero", __func__);
                pkg->invalid();
                co_return;
            }
        }

        if (pkg->header_.size == 0)
            co_return;

        if (auto [ec, len] = co_await async_read(socket, asio::buffer(pkg->data_)); ec) {
            spdlog::warn("{} - Read package data failed: {}", __func__, ec.message());
            pkg->invalid();
        }
        co_return;
    }
} // base