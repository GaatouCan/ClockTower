#pragma once

#include "common.h"
#include "ConnectionHandler.h"
#include "PackageCodec.h"
#include "PackagePool.h"
#include "TSDeque.h"

#include <memory>

class UWorldNode final : std::enable_shared_from_this<UWorldNode> {

    ATcpSocket mSocket;
    UPackagePool &mPool;
    size_t mNodeIndex;

    TSDeque<IPackage *> mOutput;

    std::unique_ptr<IPackageCodec> mCodec = nullptr;
    std::unique_ptr<IConnectionHandler> mHandler = nullptr;

    std::string mKey;

    ASystemTimer mWatchdogTimer;
    ATimePoint mDeadline;

    AThreadID mThreadId;

    uint32_t mContextNullCount = 0;
    std::any mContext;

public:
    UWorldNode() = delete;

    explicit UWorldNode(ATcpSocket socket, UPackagePool &pool);
    ~UWorldNode();
};
