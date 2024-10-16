#pragma once

#include "../TProtocolHandler.h"
#include "../../base/impl/Package.h"
#include "../protocol/ProtocolSystem.h"

namespace base {
    class ProtocolHandlerImpl final : public TProtocolHandler<ProtoFunctor> {
    public:
        awaitable<void> Execute(const std::shared_ptr<Connection> &, IPackage *) override;
    };
} // base
