#pragma once

#include "Object.h"

#include <asio.hpp>


class IPackage;
class UConnection;
class ICharacter;

using AProtoFunctor = asio::awaitable<void>(*)(const std::shared_ptr<ICharacter> &, IPackage *);

class IProtocolHandler : public UObject {
public:
    virtual asio::awaitable<void> Execute(const AProtoFunctor&, const std::shared_ptr<UConnection>&, IPackage *) = 0;
};
