#pragma once

class IPlayerComponent {

    class UComponentModule *mModule;

public:
    explicit IPlayerComponent(UComponentModule *module);
    virtual ~IPlayerComponent();

    [[nodiscard]] virtual const char *GetComponentName() const = 0;

    IPlayerComponent() = delete;

    [[nodiscard]] UComponentModule *GetModule() const;
    [[nodiscard]] class UPlayer *GetOwner() const;

    virtual void OnLogin();
    virtual void OnLogout();

    virtual void SyncCache(struct FCacheNode *node);
};
