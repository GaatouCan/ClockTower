#pragma once

#include <system/manager/manager.h>
#include <spdlog/spdlog.h>


class UPlayer;
class UCommandObject;

class UCommandManager final : public IManager {

    std::shared_ptr<spdlog::logger> mClientLogger;
    std::shared_ptr<spdlog::logger> mOperateLogger;

    ATimePoint mFetchTime;
    std::set<uint64_t> mCurrentOperateCommandSet;

public:
    explicit UCommandManager(FContextNode &ctx);
    ~UCommandManager() override;

    [[nodiscard]] constexpr const char * GetManagerName() const override {
        return "UCommandManager";
    }

    void OnTick(ATimePoint now) override;

    awaitable<void> OnClientCommand(
        const std::shared_ptr<UPlayer> &player,
        const std::string &type,
        const std::string &args);

    awaitable<void> OnOperateCommand(
        uint64_t commandID,
        uint64_t createTime,
        const std::string &creator,
        const std::string &type,
        const std::string &args);

private:
    awaitable<void> FetchOperateCommand();
};
