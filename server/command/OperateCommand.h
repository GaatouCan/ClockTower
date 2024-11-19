#pragma once

#include "AbstractCommand.h"


class IOperateCommand : public IAbstractCommand {

    uint64_t mCommandId;
    unsigned int mCreateTime;

public:
    explicit IOperateCommand(UCommandObject param);

};

