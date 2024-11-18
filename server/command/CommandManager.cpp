#include "CommandManager.h"

UCommandManager::UCommandManager(asio::io_context &ctx)
    : IManager(ctx) {
}

UCommandManager::~UCommandManager() {
}
