#pragma once

#include <map>
#include <memory>
#include <functional>

#include "../../GameWorld.h"
#include "../../PlayerID.h"

#ifdef __linux__
#include <cstdint>
#endif

class IAbstractPlayer;
class UConnection;

using AConnectionPointer = std::shared_ptr<UConnection>;
using APlayerPointer = std::shared_ptr<IAbstractPlayer>;

class UScene final {

    uint32_t mSceneID;

    std::map<uint64_t, std::shared_ptr<IAbstractPlayer>> mPlayerMap;

    static std::function<APlayerPointer(const AConnectionPointer &)> sPlayerCreator;

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

    APlayerPointer CreatePlayer(const AConnectionPointer &conn);

    void PlayerEnterScene(const APlayerPointer &player);
    void PlayerLeaveScene(const APlayerPointer &player);

    static void DefinePlayerCreator(const std::function<APlayerPointer(const AConnectionPointer &)> & creator);
};

