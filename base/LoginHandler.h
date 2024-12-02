#pragma once

#include "PlayerID.h"

#include <memory>
#include <string>
#include <asio/awaitable.hpp>


class IAbstractPlayer;
using APlayerPointer = std::shared_ptr<IAbstractPlayer>;

using asio::awaitable;

struct FLoginInfo {
    FPlayerID pid;
    std::string token;
};

class ILoginHandler {
public:
    virtual ~ILoginHandler() = default;
    virtual awaitable<FLoginInfo> ParseLoginInfo(class IPackage *) = 0;
    virtual awaitable<void> OnPlayerLogin(const APlayerPointer&, const FLoginInfo&) = 0;
};
