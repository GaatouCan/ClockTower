#pragma once

#include <mysqlx/xdevapi.h>
#include <utility>

#include "../base/Object.h"

using ADatabaseTask = std::function<void(mysqlx::Schema &)>;

class IDBCallbackWrapper : public UObject {
public:
    virtual void Execute(mysqlx::Schema &) = 0;
};

template<class Callback>
class TDBCallbackWrapper final : public IDBCallbackWrapper {
    ADatabaseTask task_;
    Callback cb_;

public:
    TDBCallbackWrapper(ADatabaseTask task, Callback &&cb)
        : task_(std::move(task)), cb_(std::forward<Callback>(cb)) {
    }

    void Execute(mysqlx::Schema &schema) override {
        std::invoke(task_, schema);
        std::invoke(std::move(cb_));
    }
};
