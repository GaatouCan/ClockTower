#pragma once

#include "../../Character.h"
#include "../../Connection.h"

#include <memory>

class IAbstractPlayer : public UCharacter, public std::enable_shared_from_this<IAbstractPlayer> {

    AConnectionPointer mConn;
    uint64_t mId;

protected:
    ATimePoint mLoginTime;
    ATimePoint mLogoutTime;

public:
    IAbstractPlayer() = delete;

    explicit IAbstractPlayer(AConnectionPointer conn);
    ~IAbstractPlayer() override;

    void SetConnection(const AConnectionPointer &conn);
    AConnectionPointer GetConnection() const;

    ATcpSocket &GetSocket() const;

    uint64_t GetPlayerID() const;

    template<typename FUNC, typename... ARGS>
    void RunInThread(FUNC &&func, ARGS &&... args) {
        co_spawn(mConn->GetSocket().get_executor(), [func = std::forward<FUNC>(func), ...args = std::forward<ARGS>(args)]() -> awaitable<void> {
            try {
                std::invoke(func, args...);
            } catch (std::exception &e) {
                spdlog::error("Player::RunInThread: {}", e.what());
            }
            co_return;
        }, detached);
    }

    IPackage *BuildPackage() const;
    void Send(IPackage *pkg) const;

    virtual void OnLogin();
    virtual void OnLogout();

    bool IsLogin() const;
};


