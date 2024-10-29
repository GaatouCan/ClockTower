#include "PackageCodecImpl.h"
#include "Package.h"

#include <spdlog/spdlog.h>

namespace base {
    awaitable<void> UPackageCodecImpl::Encode(ATcpSocket &socket, IPackage *pkg) {
        try {
            const auto tmp = dynamic_cast<FPackage *>(pkg);

            {
                auto [ec, len] = co_await async_write(socket, asio::buffer(&tmp->header_, FPackage::headerSize));
                if (ec) {
                    spdlog::warn("{} - Write package header failed: {}", __func__, ec.message());
                    tmp->Invalid();
                    co_return;
                }

                if (len == 0) {
                    spdlog::warn("{} - Write package header length equal zero", __func__);
                    tmp->Invalid();
                    co_return;
                }
            }

            if (tmp->header_.size == 0)
                co_return;

            if (auto [ec, len] = co_await async_write(socket, asio::buffer(tmp->data_)); ec) {
                spdlog::warn("{} - Write data failed: {}", __func__, ec.message());
                tmp->Invalid();
            }

        } catch (std::exception &e) {
            spdlog::error("{} - {}", __func__, e.what());
        }

        co_return;
    }

    awaitable<void> UPackageCodecImpl::Decode(ATcpSocket &socket, IPackage *pkg) {
        try {
            const auto tmp = dynamic_cast<FPackage *>(pkg);
            {
                auto [ec, len] = co_await async_read(socket, asio::buffer(&tmp->header_, FPackage::headerSize));
                if (ec) {
                    spdlog::warn("{} - Read package header failed: {}", __func__, ec.message());
                    tmp->Invalid();
                    co_return;
                }

                if (len == 0) {
                    spdlog::warn("{} - Read package header length equal zero", __func__);
                    tmp->Invalid();
                    co_return;
                }
            }

            if (tmp->header_.size == 0)
                co_return;

            tmp->data_.resize(tmp->header_.size);
            if (auto [ec, len] = co_await async_read(socket, asio::buffer(tmp->data_)); ec) {
                spdlog::warn("{} - Read package data failed: {}", __func__, ec.message());
                pkg->Invalid();
            }
        } catch (std::exception &e) {
            spdlog::error("{} - {}", __func__, e.what());
        }
        co_return;
    }
} // base