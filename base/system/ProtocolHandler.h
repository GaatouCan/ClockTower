#pragma once

#include <asio.hpp>

#include "ProtoType.h"
#include "../Object.h"

class IPackage;
class UConnection;
class ICharacter;

using protocol::EProtoType;
using AProtoFunctor = asio::awaitable<void>(*)(const std::shared_ptr<ICharacter> &, IPackage *);

class IProtocolHandler : public UObject {
public:
    virtual asio::awaitable<void> Execute(const AProtoFunctor&, const std::shared_ptr<UConnection>&, IPackage *) = 0;
};
