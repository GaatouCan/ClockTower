#pragma once

#include "./ConnectionHandlerImpl.h"
#include "Package.h"
#include "../IConnectionHandler.h"

namespace base {
    class ConnectionHandlerImpl final : public TConnectionHandler<FPackage> {
    public:
        void OnConnected(const ConnectionPointer &) override;
        void OnClosed(const ConnectionPointer &) override;

        awaitable<void> OnReadPackageT(const ConnectionPointer &conn, FPackage *pkg) override;
    };
} // base
