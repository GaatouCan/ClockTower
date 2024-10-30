#pragma once

#include "../../base/GameWorld.h"
#include "../../base/SubSystem.h"
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
        mgrMap_[typeid(T)] = new T(ctx_);
        spdlog::info("{} - Loaded Manager: {}", typeid(T).name());
    }

    template<MANAGER_TYPE T>
    T *GetManager() {
        if (const auto it = mgrMap_.find(typeid(T)); it != mgrMap_.end()) {
            return dynamic_cast<T *>(it->second);
        }
        return nullptr;
    }

    [[nodiscard]] AThreadID GetThreadID() const;
    [[nodiscard]] bool InManagerThread() const;

private:
    std::unordered_map<std::type_index, IManager *> mgrMap_;

    asio::io_context ctx_;
    ASteadyTimer timer_;

    std::thread mgrThread_;
    AThreadID tid_;
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
