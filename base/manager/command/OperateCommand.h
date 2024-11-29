#pragma once

#include "AbstractCommand.h"


class IOperateCommand : public IAbstractCommand {

    uint64_t mCommandID;
    unsigned int mCreateTime;

public:
    explicit IOperateCommand(UCommandObject param);

    void SetCommandID(uint64_t id);
};

