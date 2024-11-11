#pragma once

class IPlayerComponent {

    class UComponentModule *mModule;

protected:
    friend class UComponentModule;

    explicit IPlayerComponent(UComponentModule *module);
    virtual ~IPlayerComponent();

public:
    IPlayerComponent() = delete;

    [[nodiscard]] virtual const char * GetComponentName() const = 0;

    [[nodiscard]] UComponentModule *GetModule() const;
    [[nodiscard]] class UPlayer *GetOwner() const;

    virtual void OnLogin();
    virtual void OnLogout();

    virtual void SyncCache(struct FCacheNode *node);
};

#define COMPONENT_BODY(comp) \
    friend UComponentModule; \
public: \
    [[nodiscard]] constexpr const char * GetComponentName() const override { \
        return #comp; \
    } \
private:
