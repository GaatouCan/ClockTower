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
    template<MANAGER_TYPE T>
        void CreateManager() {
        mManagerMap[typeid(T)] = new T(mIOContext);
        spdlog::info("{} - Loaded Manager: {}", __func__, typeid(T).name());
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

private:
    std::unordered_map<std::type_index, IManager *> mManagerMap;

    asio::io_context mIOContext;
    ASteadyTimer mTickTimer;

    std::thread mManagerThread;
    AThreadID mManagerThreadId;
};

#define REGISTER_MANAGER(mgr) \
sys->CreateManager<mgr>();

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