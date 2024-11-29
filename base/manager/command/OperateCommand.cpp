#include "OperateCommand.h"

IOperateCommand::IOperateCommand(UCommandObject param)
    : IAbstractCommand(std::move(param)) {
}

void IOperateCommand::SetCommandID(const uint64_t id) {
    mCommandID = id;
}
