#pragma once

#include "CommandObject.h"

class IAbstractCommand {

protected:
    UCommandObject mParam;

public:
    IAbstractCommand() = delete;

    explicit IAbstractCommand(UCommandObject param);
    virtual ~IAbstractCommand() = default;

    virtual void Execute() = 0;
};

