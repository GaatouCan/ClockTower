#pragma once

#include <mysqlx/xdevapi.h>
#include <utility>

using ADatabaseTask = std::function<bool(mysqlx::Schema &)>;

class IDatabaseWrapper {
public:
    virtual ~IDatabaseWrapper() = default;
    virtual void Execute(mysqlx::Schema &) = 0;
};

template<typename Callback>
class TDBSelectWrapper final : public IDatabaseWrapper {

    std::string mTableName;
    std::string mWhereExpress;

    Callback mCallback;

public:
    TDBSelectWrapper() = delete;

    TDBSelectWrapper(std::string table, std::string where, Callback &&cb)
        : mTableName(std::move(table)),
          mWhereExpress(std::move(where)),
          mCallback(std::forward<Callback>(cb)) {
    }

    void Execute(mysqlx::Schema &schema) override {
        if (auto table = schema.getTable(mTableName); table.existsInDatabase()) {
            auto res = std::make_optional(table.select().where(mWhereExpress).execute());
            std::invoke(std::move(mCallback), std::move(res));
        } else {
            std::invoke(std::move(mCallback), std::nullopt);
        }
    }
};

template<typename Callback>
class UDBTaskWrapper final : public IDatabaseWrapper {

    ADatabaseTask mTask;
    Callback mCallback;

public:
    UDBTaskWrapper() = delete;
    UDBTaskWrapper(ADatabaseTask task, Callback &&cb) : mTask(std::move(task)),  mCallback(std::forward<Callback>(cb)) {}

    void Execute(mysqlx::Schema &schema) override {
        const bool ret = std::invoke(mTask, schema);
        std::invoke(std::move(mCallback), ret);
    }
};
