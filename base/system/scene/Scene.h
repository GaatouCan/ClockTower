#pragma once

#include <map>
#include <memory>
#include <functional>

#include "../../GameWorld.h"

#ifdef __linux__
#include <cstdint>
#endif

class IAbstractPlayer;
class UConnection;

class UScene final {

    uint32_t mSceneID;

    std::map<uint64_t, std::shared_ptr<IAbstractPlayer>> mPlayerMap;

    static std::function<std::shared_ptr<IAbstractPlayer>(const std::shared_ptr<UConnection> &)> sPlayerCreator;

public:
    UScene();
    ~UScene();

    void SetSceneID(uint32_t id);
    [[nodiscard]] uint32_t GetSceneID() const;

    template<typename FUNC, typename... ARGS>
    void RunInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(GetWorld().GetIOContext(), [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            try {
                std::invoke(func, args...);
            } catch (std::exception &e) {
                spdlog::error("Player::RunInThread: {}", e.what());
            }
            co_return;
        }, detached);
    }

    std::shared_ptr<IAbstractPlayer> CreatePlayer(const std::shared_ptr<UConnection> &conn);

    void PlayerEnterScene(const std::shared_ptr<IAbstractPlayer> &player);
    void PlayerLeaveScene(const std::shared_ptr<IAbstractPlayer> &player);

    static void DefinePlayerCreator(const std::function<std::shared_ptr<IAbstractPlayer>(const std::shared_ptr<UConnection> &)>& creator);
};

