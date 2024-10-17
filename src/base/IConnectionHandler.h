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
        virtual void OnReadPackage(const ConnectionPointer &conn, IPackage *pkg) {}
        virtual void OnWritePackage(const ConnectionPointer &) {}
        virtual void OnClosed(const ConnectionPointer &) {}
    };

    template<PACKAGE_TYPE T>
    class TConnectionHandler : public IConnectionHandler {
    public:
        void OnReadPackage(const ConnectionPointer &conn, IPackage *pkg) override {
            OnReadPackageT(conn, dynamic_cast<T *>(pkg));
        }

        virtual void OnReadPackageT(const ConnectionPointer &conn, T *pkg) {}
    };
}
