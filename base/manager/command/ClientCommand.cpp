#include "ClientCommand.h"

IClientCommand::IClientCommand(UCommandObject param)
    : IAbstractCommand(std::move(param)), mSender(0) {
}

void IClientCommand::SetSender(const uint64_t sender) {
    mSender = sender;
}

uint64_t IClientCommand::GetSender() const {
    return mSender;
}
