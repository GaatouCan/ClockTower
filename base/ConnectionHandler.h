#pragma once

#include <memory>
#include <asio/awaitable.hpp>

#include "Package.h"

class UConnection;
using AConnectionPointer = std::shared_ptr<UConnection>;
using asio::awaitable;

class IConnectionHandler : public UObject {
public:
    virtual void OnConnected(const AConnectionPointer &) {}

    virtual awaitable<void> OnReadPackage(const AConnectionPointer &, IPackage *) { co_return; }
    virtual awaitable<void> OnWritePackage(const AConnectionPointer &) { co_return; }

    virtual void OnClosed(const AConnectionPointer &) {}
};