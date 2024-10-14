#pragma once

#include "../IProtocolHandler.h"

#include "../../base/Connection.h"
#include "../../base/impl/Package.h"

namespace base {
    class ProtocolHandlerImpl final : public TProtocolHandler<Connection, Package> {
    public:
        awaitable<void> execute(const std::shared_ptr<Connection> &, Package *) override;
    };
} // base
