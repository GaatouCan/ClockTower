#pragma once

#include "../../GameWorld.h"
#include "../../SubSystem.h"
#include "Manager.h"

#include <typeindex>

class UManagerSystem final : public ISubSystem {

    SUB_SYSTEM_BODY(ManagerSystem)

    UManagerSystem();
    ~UManagerSystem() override;

    void Init() override;

public:
    void SetManagerLoader(const std::function<void(UManagerSystem *)> &loader);

    template<MANAGER_TYPE T>
    T* CreateManager() {
        auto mgr = new T(mIOContext);
        mManagerMap[typeid(T)] = mgr;
        spdlog::info("{} - Loaded {}", __func__, mgr->GetManagerName());
        return mgr;
    }

    template<MANAGER_TYPE T>
    T *GetManager() {
        if (const auto it = mManagerMap.find(typeid(T)); it != mManagerMap.end()) {
            return dynamic_cast<T *>(it->second);
        }
        return nullptr;
    }

    [[nodiscard]] AThreadID GetThreadID() const;
    [[nodiscard]] bool InManagerThread() const;

    [[nodiscard]] bool IsTimeToLoadManager() const;

private:
    std::unordered_map<std::type_index, IManager *> mManagerMap;
    std::function<void(UManagerSystem *)> mLoader;

    asio::io_context mIOContext;
    ASteadyTimer mTickTimer;

    std::thread mManagerThread;
    AThreadID mManagerThreadId;
    bool bCouldLoad = false;
};

template<MANAGER_TYPE T>
T *GetManager() {
    const auto sys = GetSystem<UManagerSystem>();
    if (sys == nullptr) {
        spdlog::critical("{} - Failed to found ManagerSystem.", __func__);
        GetWorld().Shutdown();
        exit(-1);
    }

    return sys->GetManager<T>();
}
