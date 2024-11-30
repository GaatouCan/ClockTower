#pragma once

#include <map>
#include <memory>
#include <functional>

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

    std::shared_ptr<IAbstractPlayer> CreatePlayer(const std::shared_ptr<UConnection> &conn);

    void PlayerEnterScene(const std::shared_ptr<IAbstractPlayer>& player);
    void PlayerLeaveScene(const std::shared_ptr<IAbstractPlayer>& player);

    static void DefinePlayerCreator(const std::function<std::shared_ptr<IAbstractPlayer>(const std::shared_ptr<UConnection> &)>& creator);
};

