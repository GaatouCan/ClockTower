#pragma once

#include "PlayerID.h"

#include <memory>
#include <string>
#include <asio/awaitable.hpp>


class UConnection;
class IAbstractPlayer;

using asio::awaitable;

struct FLoginInfo {
    FPlayerID pid;
    std::string token;
};

class ILoginHandler {
public:
    virtual ~ILoginHandler() = default;
    virtual awaitable<FLoginInfo> ParseLoginInfo(class IPackage *) = 0;
    virtual awaitable<std::shared_ptr<IAbstractPlayer>> OnPlayerLogin(const std::shared_ptr<UConnection>&, const FLoginInfo&) = 0;
};
