#pragma once

#include "AbstractCommand.h"


class IOperateCommand : public IAbstractCommand {

public:
    explicit IOperateCommand(UCommandObject param);

};

