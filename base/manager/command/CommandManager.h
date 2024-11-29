#pragma once

#include "../../system/manager/Manager.h"

#include "ClientCommand.h"
#include "OperateCommand.h"

#include <spdlog/spdlog.h>

class IAbstractPlayer;
class UCommandObject;

class UCommandManager final : public IManager {

    using ACommandCreator = std::function<std::shared_ptr<IAbstractCommand>(const UCommandObject&)>;

    static std::function<void(UCommandManager*)> sClientCommandRegister;
    static std::function<void(UCommandManager*)> sOperateCommandRegister;

    std::unordered_map<std::string, ACommandCreator> mOperateCommandMap;
    std::unordered_map<std::string, ACommandCreator> mClientCommandMap;

    std::shared_ptr<spdlog::logger> mClientLogger;
    std::shared_ptr<spdlog::logger> mOperateLogger;

public:
    explicit UCommandManager(FContextNode &ctx);
    ~UCommandManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UCommandManager";
    }

    static void SetClientCommandRegister(const std::function<void(UCommandManager*)> &func);
    static void SetOperateCommandRegister(const std::function<void(UCommandManager*)> &func);

    template<class T>
    requires std::derived_from<T, IClientCommand>
    void RegisterClientCommand(const std::string &cmd) {
        if (!mClientCommandMap.contains(cmd)) {
            mClientCommandMap[cmd] = [](const UCommandObject &obj) -> IAbstractCommand* {
                return std::make_shared<T>(obj);
            };
        }
    }

    template<class T>
    requires std::derived_from<T, IOperateCommand>
    void RegisterOperateCommand(const std::string &cmd) {
        if (!mOperateCommandMap.contains(cmd)) {
            mOperateCommandMap[cmd] = [](const UCommandObject &obj) -> IAbstractCommand* {
                return new T(obj);
            };
        }
    }

    void OnTick(ATimePoint now) override;

    awaitable<void> OnClientCommand(
        const std::shared_ptr<IAbstractPlayer> &player,
        const std::string &type,
        const std::string &args);

    awaitable<void> OnOperateCommand(
        uint64_t commandID,
        uint64_t createTime,
        const std::string &creator,
        const std::string &type,
        const std::string &args);
};
