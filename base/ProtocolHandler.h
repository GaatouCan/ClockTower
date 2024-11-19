#pragma once

#include <asio.hpp>


class IPackage;
class UConnection;
class IAbstractPlayer;

using AProtoFunctor = asio::awaitable<void>(*)(const std::shared_ptr<IAbstractPlayer> &, IPackage *);

class IProtocolHandler {
public:
    virtual ~IProtocolHandler() = default;
    virtual asio::awaitable<void> Execute(const AProtoFunctor&, const std::shared_ptr<UConnection>&, IPackage *) = 0;
};
