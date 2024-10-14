#pragma once

#include "./ConnectionHandlerImpl.h"
#include "Package.h"
#include "../IConnectionHandler.h"

namespace base {
    class ConnectionHandlerImpl final : public TConnectionHandler<Package> {
    public:
        void onConnected(const ConnectionPointer &) override;
        void onClosed(const ConnectionPointer &) override;
        awaitable<void> onReadPackageT(const ConnectionPointer &conn, Package *pkg) override;
    };
} // base
