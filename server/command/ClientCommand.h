#pragma once

#include "AbstractCommand.h"


class IClientCommand : public IAbstractCommand {

public:
    explicit IClientCommand(UCommandObject param);

};

