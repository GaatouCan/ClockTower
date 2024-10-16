#pragma once

#include <memory>
#include <asio/awaitable.hpp>

#include "IPackage.h"

namespace base {
    class Connection;
    using ConnectionPointer = std::shared_ptr<Connection>;
    using asio::awaitable;

    class IConnectionHandler {
    public:
        virtual ~IConnectionHandler() = default;

        virtual void OnConnected(const ConnectionPointer &) {}
        virtual awaitable<void> OnReadPackage(const ConnectionPointer &conn, IPackage *pkg) { co_return; }
        virtual awaitable<void> OnWritePackage(const ConnectionPointer &) { co_return; }
        virtual void OnClosed(const ConnectionPointer &) {}
    };

    template<PACKAGE_TYPE T>
    class TConnectionHandler : public IConnectionHandler {
    public:
        awaitable<void> OnReadPackage(const ConnectionPointer &conn, IPackage *pkg) override {
            co_await OnReadPackageT(conn, dynamic_cast<T *>(pkg));
        }

        virtual awaitable<void> OnReadPackageT(const ConnectionPointer &conn, T *pkg) { co_return; }
    };
}
