#pragma once

#include "common.h"
#include "package_pool.h"

struct FContextNode {
    UPackagePool mPool;
    asio::io_context mIOContext;
    AThreadID mThreadID;
    size_t mIndex; // From 0
};
