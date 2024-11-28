#pragma once

#include <ProtocolHandler.h>

class UProtocolHandlerImpl final : public IProtocolHandler {
public:
    awaitable<void> Execute(const AProtoFunctor &, const std::shared_ptr<UConnection> &, IPackage *) override;
};
