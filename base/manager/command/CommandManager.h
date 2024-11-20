#pragma once

#include "../../system/manager/Manager.h"

#include "ClientCommand.h"
#include "OperateCommand.h"

class IAbstractPlayer;
class UCommandObject;


class UCommandManager final : public IManager {

    using ACommandCreator = std::function<IAbstractCommand*(const UCommandObject&)>;

    std::unordered_map<std::string, ACommandCreator> mOperateCmdMap;
    std::unordered_map<std::string, ACommandCreator> mClientCmdMap;

public:
    explicit UCommandManager(asio::io_context &ctx);
    ~UCommandManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UCommandManager";
    }

    template<class T>
    requires std::derived_from<T, IClientCommand>
    void RegisterClientCommand(const std::string &cmd) {
        if (!mClientCmdMap.contains(cmd)) {
            mClientCmdMap[cmd] = [](const UCommandObject &obj) -> IAbstractCommand* {
                return new T(obj);
            };
        }
    }

    template<class T>
    requires std::derived_from<T, IOperateCommand>
    void RegisterOperateCommand(const std::string &cmd) {
        if (!mOperateCmdMap.contains(cmd)) {
            mOperateCmdMap[cmd] = [](const UCommandObject &obj) -> IAbstractCommand* {
                return new T(obj);
            };
        }
    }


    void OnClientCommand(const std::shared_ptr<IAbstractPlayer> &player, const std::string &type, const std::string &args);
};
