#pragma once

#include "../../sub_system.h"
#include "client_command.h"
#include "operate_command.h"

#include <set>
#include <spdlog/spdlog.h>

class UCommandSystem final : public ISubSystem {

    using ACommandCreator = std::function<std::shared_ptr<IAbstractCommand>(const UCommandObject&)>;

    std::unordered_map<std::string, ACommandCreator> mOperateCommandMap;
    std::unordered_map<std::string, ACommandCreator> mClientCommandMap;

public:
    UCommandSystem();
    ~UCommandSystem() override;

    void Init() override;

    [[nodiscard]] constexpr const char *GetSystemName() const override { return "UCommandSystem"; }

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

    std::shared_ptr<IClientCommand> CreateClientCommand(const std::string &cmd, const std::string &args) const;
    std::shared_ptr<IOperateCommand> CreateOperateCommand(const std::string &cmd, const std::string &args) const;
};
