#pragma once

#include <mysqlx/xdevapi.h>
#include <memory>
#include <utility>

using ADatabaseTask = std::function<void(mysqlx::Schema &)>;

class IDatabaseWrapper {
public:
    virtual ~IDatabaseWrapper() = default;
    virtual void Execute(mysqlx::Schema &) = 0;
};

template<class Callback>
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
            auto res = std::make_shared<mysqlx::RowResult>(table.select().where(mWhereExpress).execute());
            std::invoke(std::move(mCallback), res);
        } else
            std::invoke(std::move(mCallback), nullptr);
    }
};

class UDBTaskWrapper final : public IDatabaseWrapper {

    ADatabaseTask mTask;

public:
    UDBTaskWrapper() = delete;
    explicit UDBTaskWrapper(ADatabaseTask task) : mTask(std::move(task)) {}

    void Execute(mysqlx::Schema &schema) override {
        std::invoke(mTask, schema);
    }
};
