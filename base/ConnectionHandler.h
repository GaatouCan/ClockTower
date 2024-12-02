#pragma once

#include <memory>
#include <asio/awaitable.hpp>


class UConnection;
class IPackage;

using AConnectionPointer = std::shared_ptr<UConnection>;
using asio::awaitable;

class IConnectionHandler {
protected:
    std::weak_ptr<UConnection> mConn;
public:
    IConnectionHandler() = delete;
    explicit IConnectionHandler(const AConnectionPointer &conn) : mConn(conn) {}
    virtual ~IConnectionHandler() = default;

    virtual void OnConnected() {}

    virtual awaitable<void> OnReadPackage(IPackage *) { co_return; }
    virtual awaitable<void> OnWritePackage(IPackage *) { co_return;}

    virtual void OnClosed() {}
};
