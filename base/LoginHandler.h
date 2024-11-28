#pragma once

#include <memory>
#include <string>
#include <asio/awaitable.hpp>

class IPackage;
class UConnection;

using asio::awaitable;

struct FLoginInfo {
    uint64_t pid = 0;
    std::string token;
};

class ILoginHandler {
public:
    virtual ~ILoginHandler() = default;
    virtual awaitable<FLoginInfo> ParseLoginInfo(IPackage *) = 0;
    virtual awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection>&, const FLoginInfo&) = 0;
};
