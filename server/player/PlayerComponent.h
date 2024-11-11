#pragma once

class IPlayerComponent {

    class UComponentModule *mModule;

public:
    IPlayerComponent() = delete;

    explicit IPlayerComponent(UComponentModule *module);
    virtual ~IPlayerComponent();

    [[nodiscard]] UComponentModule *GetModule() const;
    [[nodiscard]] class UPlayer *GetOwner() const;

    virtual void OnLogin();
    virtual void OnLogout();

    virtual void SyncCache(struct FCacheNode *node);
};
