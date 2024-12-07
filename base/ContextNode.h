#pragma once

#include "common.h"
#include "PackagePool.h"

/**
 * 每个线程节点数据
 */
struct FContextNode {
    UPackagePool pool;
    AIOContext ctx;
    AThreadID tid;
    size_t index; // From 0
};
