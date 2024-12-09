#pragma once

#include <protocol_handler.h>

class UProtocolHandlerImpl final : public IProtocolHandler {
public:
    awaitable<void> Execute(const AProtoFunctor &, const std::shared_ptr<UConnection> &, IPackage *) override;
};
