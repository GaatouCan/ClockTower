#include "reactor.h"

UReactor::UReactor()
    : mTaskQueue(nullptr) {
}

void UReactor::SetTaskQueue(UTaskQueue *taskQueue) {
    mTaskQueue = taskQueue;
}

UTaskQueue * UReactor::GetTaskQueue() const {
    return mTaskQueue;
}
