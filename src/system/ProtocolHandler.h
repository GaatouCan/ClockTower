#pragma once

#include "../../protobuf/ProtoType.generated.h"

#include <asio.hpp>


using protocol::EProtoType;
using AProtoFunctor = asio::awaitable<void>(*)(const std::shared_ptr<class ICharacter> &, class IPackage *);

class IProtocolHandler {
public:
    virtual ~IProtocolHandler() = default;
    virtual asio::awaitable<void> Execute(const AProtoFunctor &, const std::shared_ptr<class UConnection> &, IPackage *) = 0;
};
