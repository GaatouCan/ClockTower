#pragma once

#include "../../GameWorld.h"
#include "../../SubSystem.h"
#include "Manager.h"

#include <typeindex>

inline std::vector<std::function<IManager*(class UManagerSystem *sys)>> gManagerCreatorVector;

class UManagerSystem final : public ISubSystem {

    std::unordered_map<std::type_index, IManager *> mManagerMap;

    asio::io_context mIOContext;
    ASteadyTimer mTickTimer;

    std::thread mManagerThread;
    AThreadID mManagerThreadId;

public:

    template<MANAGER_TYPE T>
    class TManagerRegister {
    public:
        TManagerRegister() {
            gManagerCreatorVector.emplace_back([](UManagerSystem *sys) {
                return sys->CreateManager<T>();
            });
        }
    };

    UManagerSystem();
    ~UManagerSystem() override;

    void Init() override;

    [[nodiscard]] constexpr const char * GetSystemName() const override {
        return "UManagerSystem";
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
    template<MANAGER_TYPE T>
    T* CreateManager() {
        auto mgr = new T(mIOContext);
        mManagerMap[typeid(T)] = mgr;
        spdlog::info("{} - Loaded {}", __func__, mgr->GetManagerName());
        return mgr;
    }
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

#define REGISTER_MANAGER(mgr) \
static UManagerSystem::TManagerRegister<mgr> g_Manager_##mgr##_Register();