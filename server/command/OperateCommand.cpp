#include "OperateCommand.h"

IOperateCommand::IOperateCommand(UCommandObject param)
    : IAbstractCommand(std::move(param)) {
}
