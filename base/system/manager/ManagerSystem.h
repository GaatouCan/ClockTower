#pragma once

#include "../../game_world.h"
#include "Manager.h"

#include <typeindex>

class UManagerSystem final : public ISubSystem {

    std::unordered_map<std::type_index, IManager *> mManagerMap;
    std::function<void(UManagerSystem *)> mManagerLoader;

    FContextNode mContextNode;
    ASystemTimer mTickTimer;

    std::thread mManagerThread;

public:
    UManagerSystem();
    ~UManagerSystem() override;

    void Init() override;

    [[nodiscard]] constexpr const char * GetSystemName() const override {
        return "UManagerSystem";
    }

    void SetManagerLoader(const std::function<void(UManagerSystem *)> &loader);

    template<MANAGER_TYPE T>
    T* CreateManager() {
        auto mgr = new T(mContextNode);
        mManagerMap[typeid(T)] = mgr;
        spdlog::info("\tLoaded {}", mgr->GetManagerName());
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
};

template<MANAGER_TYPE T>
T *GetManager() {
    const auto sys = GetSystem<UManagerSystem>();
    if (sys == nullptr) {
        spdlog::critical("{} - Failed to found ManagerSystem.", __FUNCTION__);
        GetWorld().Shutdown();
        exit(-1);
    }

    return sys->GetManager<T>();
}
