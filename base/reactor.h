#pragma once

#include "common.h"


class UReactor {

    class UTaskQueue *mTaskQueue;

public:
    UReactor();
    virtual ~UReactor() = default;

    DISABLE_COPY_MOVE(UReactor)

    void SetTaskQueue(UTaskQueue *taskQueue);
    [[nodiscard]] UTaskQueue *GetTaskQueue() const;
};
