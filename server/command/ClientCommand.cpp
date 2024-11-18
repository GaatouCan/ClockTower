#include "ClientCommand.h"

IClientCommand::IClientCommand(UCommandObject param)
    : IAbstractCommand(std::move(param)) {
}
