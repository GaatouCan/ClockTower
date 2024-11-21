#pragma once

#include <ConnectionHandler.h>


class UConnectionHandlerImpl final : public IConnectionHandler {
public:
    void OnConnected(const AConnectionPointer &) override;
    void OnClosed(const AConnectionPointer &) override;
    void OnReadPackage(const AConnectionPointer &conn, IPackage *pkg) override;
};
