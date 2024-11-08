#pragma once

#include "Object.h"

#include <asio.hpp>

class IPackage;
class UConnection;

struct FLoginInfo {
    uint64_t pid = 0;
    std::string token;
};

class ILoginHandler : public UObject {
public:
    virtual FLoginInfo ParseLoginInfo(IPackage *) = 0;
    virtual asio::awaitable<void> OnPlayerLogin(const std::shared_ptr<UConnection>&, const FLoginInfo&) = 0;
};
