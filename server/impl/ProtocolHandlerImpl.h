#pragma once

#include "../../base/ProtocolHandler.h"

class UProtocolHandlerImpl final : public IProtocolHandler {
public:
    asio::awaitable<void> Execute(const AProtoFunctor &, const std::shared_ptr<UConnection> &, IPackage *) override;
};
