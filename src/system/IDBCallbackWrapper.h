#pragma once

#include <mysqlx/xdevapi.h>
#include <utility>

namespace base {
    using DBTask = std::function<void(mysqlx::Schema &)>;

    class IDBCallbackWrapper {
    public:
        virtual ~IDBCallbackWrapper() = default;

        virtual void Execute(mysqlx::Schema &) = 0;
    };

    template<class Callback>
    class TDBCallbackWrapper final : public IDBCallbackWrapper {
        DBTask task_;
        Callback cb_;

    public:
        TDBCallbackWrapper(DBTask task, Callback && cb)
            : task_(std::move(task)), cb_(std::forward<Callback>(cb)) {

        }

        void Execute(mysqlx::Schema &schema) override {
            std::invoke(task_, schema);
            std::invoke(std::move(cb_));
        }
    };
}