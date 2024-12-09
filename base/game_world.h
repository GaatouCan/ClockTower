#pragma once

#include "common.h"
#include "multi_context_pool.h"
#include "connection.h"
#include "sub_system.h"

#include <typeindex>
#include <absl/random/random.h>

class UGameWorld final {

    AIOContext mContext;
    ATcpAcceptor mAcceptor;

    UMultiContextPool mPool;

    std::map<std::string, AConnectionPointer> mConnectionMap;
    absl::BitGen mBitGen;

    ASystemTimer mFullTimer;

    struct FSystemPriority {
        int mPriority;
        std::type_index mtTypeIndex;

        bool operator<(const FSystemPriority &other) const {
            return mPriority < other.mPriority;
        }

        bool operator>(const FSystemPriority &other) const {
            return mPriority > other.mPriority;
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
    UGameWorld();
    ~UGameWorld();

    DISABLE_COPY_MOVE(UGameWorld)

    UGameWorld &Init();
    UGameWorld &Run();
    UGameWorld &Shutdown();

    void FilterConnection(const std::function<void(const AConnectionPointer &)> &filter);

    void RemoveConnection(const std::string &key);

    FContextNode &NextContextNode();
    AIOContext &GetIOContext();

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