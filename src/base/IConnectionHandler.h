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

        virtual void onConnected(const ConnectionPointer &) {}
        virtual awaitable<void> onReadPackage(const ConnectionPointer &conn, IPackage *pkg) { co_return; }
        virtual awaitable<void> onWritePackage(const ConnectionPointer &) { co_return; }
        virtual void onClosed(const ConnectionPointer &) {}
    };

    template<PACKAGE_TYPE T>
    class TConnectionHandler : public IConnectionHandler {
    public:
        awaitable<void> onReadPackage(const ConnectionPointer &conn, IPackage *pkg) override {
            co_await onReadPackageT(conn, dynamic_cast<T *>(pkg));
        }

        virtual awaitable<void> onReadPackageT(const ConnectionPointer &conn, T *pkg) { co_return; }
    };
}
