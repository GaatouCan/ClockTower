#include "abstract_command.h"

#include <utility>

IAbstractCommand::IAbstractCommand(UCommandObject param)
    :mParam(std::move(param)){
}
