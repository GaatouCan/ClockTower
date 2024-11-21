#pragma once

#include <memory>
#include <asio/awaitable.hpp>

#include "Package.h"

class UConnection;
using AConnectionPointer = std::shared_ptr<UConnection>;
using asio::awaitable;

class IConnectionHandler {
public:
    virtual ~IConnectionHandler() = default;

    virtual void OnConnected(const AConnectionPointer &) {}

    virtual void OnReadPackage(const AConnectionPointer &, IPackage *) {}
    virtual void OnWritePackage(const AConnectionPointer &) {}

    virtual void OnClosed(const AConnectionPointer &) {}
};
