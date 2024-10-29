#pragma once

#include <memory>
#include <asio/awaitable.hpp>

#include "IPackage.h"

namespace base {
    class UConnection;
    using AConnectionPointer = std::shared_ptr<UConnection>;
    using asio::awaitable;

    class IConnectionHandler {
    public:
        virtual ~IConnectionHandler() = default;

        virtual void OnConnected(const AConnectionPointer &) {}
        virtual awaitable<void> OnReadPackage(const AConnectionPointer &conn, IPackage *pkg) { co_return; }
        virtual awaitable<void> OnWritePackage(const AConnectionPointer &) { co_return; }
        virtual void OnClosed(const AConnectionPointer &) {}
    };
}
