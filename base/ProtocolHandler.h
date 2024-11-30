#pragma once

#include <asio/awaitable.hpp>


class IPackage;
class UConnection;
class UCharacter;

using asio::awaitable;
using AProtoFunctor = awaitable<void>(*)(const std::shared_ptr<UCharacter> &, IPackage *);

class IProtocolHandler {
public:
    virtual ~IProtocolHandler() = default;
    virtual awaitable<void> Execute(const AProtoFunctor&, const std::shared_ptr<UConnection>&, IPackage *) = 0;
};
