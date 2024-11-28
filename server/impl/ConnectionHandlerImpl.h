#pragma once

#include <ConnectionHandler.h>


class UConnectionHandlerImpl final : public IConnectionHandler {
public:
    explicit UConnectionHandlerImpl(const AConnectionPointer &conn) : IConnectionHandler(conn) {}

    void OnConnected() override;
    void OnClosed() override;
    void OnReadPackage(IPackage *pkg) override;
};
