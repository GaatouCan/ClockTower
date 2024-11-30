#pragma once

#include "CommandObject.h"

#include <asio.hpp>

class IAbstractCommand {

protected:
    UCommandObject mParam;

public:
    IAbstractCommand() = delete;

    explicit IAbstractCommand(UCommandObject param);
    virtual ~IAbstractCommand() = default;

    virtual asio::awaitable<bool> Execute() = 0;
};

