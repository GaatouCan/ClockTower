#pragma once

#include <mysqlx/xdevapi.h>
#include <memory>
#include <utility>

using ARowResultPointer = std::shared_ptr<mysqlx::RowResult>;
using ADatabaseTask = std::function<mysqlx::RowResult(mysqlx::Schema &)>;

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
        auto res = std::make_shared<mysqlx::RowResult>(std::invoke(mTask, schema));
        std::invoke(std::move(mCallback), res);
    }
};
