#pragma once

#include "AbstractCommand.h"


class IOperateCommand : public IAbstractCommand {

    uint64_t mCommandId;

public:
    explicit IOperateCommand(UCommandObject param);

};

