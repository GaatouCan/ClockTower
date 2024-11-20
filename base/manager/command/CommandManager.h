#pragma once

#include "../../system/manager/Manager.h"

#include "ClientCommand.h"
#include "OperateCommand.h"

class IAbstractPlayer;
class UCommandObject;

class UCommandManager final : public IManager {

    using ACommandCreator = std::function<IAbstractCommand*(const UCommandObject&)>;

    static std::function<void(UCommandManager*)> sClientCommandRegister;
    static std::function<void(UCommandManager*)> sOperateCommandRegister;

    std::unordered_map<std::string, ACommandCreator> mOperateCommandMap;
    std::unordered_map<std::string, ACommandCreator> mClientCommandMap;

public:
    explicit UCommandManager(asio::io_context &ctx);
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
                return new T(obj);
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


    void OnClientCommand(const std::shared_ptr<IAbstractPlayer> &player, const std::string &type, const std::string &args);
};
