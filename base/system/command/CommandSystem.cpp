#include "CommandSystem.h"

UCommandSystem::UCommandSystem() {
}

UCommandSystem::~UCommandSystem() {
}

void UCommandSystem::Init() {
}

std::shared_ptr<IAbstractCommand> UCommandSystem::CreateClientCommand(const std::string &cmd, const std::string &args) const {
    if (const auto it = mClientCommandMap.find(cmd); it != mClientCommandMap.end()) {
        UCommandObject obj(cmd, args);
        auto cmdPtr = std::invoke(it->second, obj);
        return cmdPtr;
    }
    return nullptr;
}

std::shared_ptr<IAbstractCommand> UCommandSystem::CreateOperateCommand(const std::string &cmd, const std::string &args) const {
    if (const auto it = mOperateCommandMap.find(cmd); it != mOperateCommandMap.end()) {
        UCommandObject obj(cmd, args);
        auto cmdPtr = std::invoke(it->second, obj);
        return cmdPtr;
    }
    return nullptr;
}
