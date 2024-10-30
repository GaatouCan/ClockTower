#pragma once

#include "../ProtocolHandler.h"
#include "../protocol/ProtocolSystem.h"


class ProtocolHandlerImpl final : public IProtocolHandler {
public:
    awaitable<void> Execute(const AProtoFunctor &, const std::shared_ptr<class UConnection> &, IPackage *) override;
};
