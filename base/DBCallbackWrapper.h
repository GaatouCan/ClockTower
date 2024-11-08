#pragma once

#include <mysqlx/xdevapi.h>
#include <utility>

using ADatabaseTask = std::function<void(mysqlx::Schema &)>;

class IDBCallbackWrapper {
public:
    virtual ~IDBCallbackWrapper() = default;
    virtual void Execute(mysqlx::Schema &) = 0;
};

template<class Callback>
class TDBCallbackWrapper final : public IDBCallbackWrapper {
    ADatabaseTask mTask;
    Callback mCallback;

public:
    TDBCallbackWrapper(ADatabaseTask task, Callback &&cb)
        : mTask(std::move(task)), mCallback(std::forward<Callback>(cb)) {
    }

    void Execute(mysqlx::Schema &schema) override {
        std::invoke(mTask, schema);
        std::invoke(std::move(mCallback));
    }
};
