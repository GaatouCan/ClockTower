#include "AbstractCommand.h"

#include <utility>

IAbstractCommand::IAbstractCommand(UCommandObject param)
    :mParam(std::move(param)){
}
