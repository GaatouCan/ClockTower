#pragma once

#include "common.h"

#include <map>
#include <memory>

class UWorldNode;

class UCrossWorld final {

    AIOContext mContext;
    ATcpAcceptor mAcceptor;

    std::map<uint32_t, std::shared_ptr<UWorldNode>> mWorldMap;

    std::atomic_bool bRunning;

public:
    UCrossWorld();
    ~UCrossWorld();

    UCrossWorld &Init();
    UCrossWorld &Run();
    UCrossWorld &Shutdown();

private:
    awaitable<void> WaitForWorld();
};

inline UCrossWorld &GetCross() {
    static UCrossWorld cross;
    return cross;
}
