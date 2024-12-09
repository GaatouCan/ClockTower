#pragma once

#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <shared_mutex>

#include "../../game_world.h"
#include "../../PlayerID.h"

#ifdef __linux__
#include <cstdint>
#endif

class IAbstractPlayer;
class UConnection;

using AConnectionPointer = std::shared_ptr<UConnection>;

class UScene final {

    uint32_t mSceneID;

    std::map<uint32_t, std::shared_ptr<IAbstractPlayer>> mPlayerMap;
    std::mutex mMutex;
    mutable std::shared_mutex mSharedMutex;

public:
    UScene() = delete;

    explicit UScene(uint32_t id);
    ~UScene();

    void SetSceneID(uint32_t id);
    [[nodiscard]] uint32_t GetSceneID() const;

    template<typename FUNC, typename... ARGS>
    void RunInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(GetWorld().GetIOContext(), [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            try {
                std::invoke(func, args...);
            } catch (std::exception &e) {
                spdlog::error("UScene::RunInThread: {}", e.what());
            }
            co_return;
        }, detached);
    }

    void PlayerEnterScene(const std::shared_ptr<IAbstractPlayer> &player);
    void PlayerLeaveScene(const std::shared_ptr<IAbstractPlayer> &player, bool bChange = false);

    std::shared_ptr<IAbstractPlayer> GetPlayer(uint32_t pid) const;

    void BroadCast(IPackage *pkg, const std::set<uint32_t> &except = {});
};

