#pragma once

#include "common.h"
#include "MultiContextPool.h"
#include "Connection.h"
#include "SubSystem.h"

#include <typeindex>


inline std::vector<std::function<ISubSystem*(UGameWorld&)>> gSubSystemCreatorVector;

class UGameWorld final {

    asio::io_context mContext;
    ATcpAcceptor mAcceptor;

    UMultiContextPool mPool;

    std::map<std::string, AConnectionPointer> mConnectionMap;
    ASteadyTimer mFullTimer;

    struct FSystemPriority {
        int priority;
        std::type_index type;

        bool operator<(const FSystemPriority &other) const {
            return priority < other.priority;
        }

        bool operator>(const FSystemPriority &other) const {
            return priority > other.priority;
        }
    };

    std::priority_queue<FSystemPriority, std::vector<FSystemPriority>, std::greater<> > mInitPriority;
    std::priority_queue<FSystemPriority, std::vector<FSystemPriority>, std::less<> > mDestPriority;
    std::unordered_map<std::type_index, ISubSystem *> mSystemMap;

    std::function<void(const AConnectionPointer &)> mConnectionFilter;

    AThreadID mMainThreadId;

    bool bInited;
    std::atomic_bool bRunning;

public:

    template<SYSTEM_TYPE T>
    class TSubSystemRegister {
    public:
        explicit TSubSystemRegister(const int priority) {
            gSubSystemCreatorVector.emplace_back([priority](UGameWorld &world) {
                return world.CreateSystem<T>(priority);
            });
        }
    };

    UGameWorld();
    ~UGameWorld();

    DISABLE_COPY_MOVE(UGameWorld)

    UGameWorld &Init();
    UGameWorld &Run();
    UGameWorld &Shutdown();

    void FilterConnection(const std::function<void(const AConnectionPointer &)> &filter);

    void RemoveConnection(const std::string &key);

    FContextNode &NextContextNode();
    asio::io_context &GetIOContext();

    AThreadID GetThreadID() const;

    template<SYSTEM_TYPE T>
    T *GetSystem() noexcept {
        if (const auto iter = mSystemMap.find(typeid(T)); iter != mSystemMap.end())
            return dynamic_cast<T *>(iter->second);
        return nullptr;
    }

private:
    awaitable<void> WaitForConnect();

    template<SYSTEM_TYPE T>
    T *CreateSystem(const int priority = 0) {
        if (bInited)
            return nullptr;

        const auto res = new T();

        mSystemMap.insert_or_assign(typeid(T), res);
        mInitPriority.push({priority, typeid(T)});
        mDestPriority.push({priority, typeid(T)});

        return res;
    }
};

inline UGameWorld &GetWorld() {
    static UGameWorld world;
    return world;
}

template<SYSTEM_TYPE T>
T *GetSystem() {
    return GetWorld().GetSystem<T>();
}

#define REGISTER_SUBSYSTEM(sys, priority) \
static UGameWorld::TSubSystemRegister<sys> g_##sys##_Register(priority);