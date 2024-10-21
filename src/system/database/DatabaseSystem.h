#pragma once

#include "../../base/ISubSystem.h"
#include "../../base/TSDeque.h"

#include <thread>
#include <vector>
#include <mysqlx/xdevapi.h>

namespace base {

    using DBTask = std::function<void(mysqlx::Schema &)>;

    struct DBSystemNode {
        std::thread th;
        ThreadID tid;
        mysqlx::Session sess;
        TSDeque<DBTask> queue;
    };

    class DatabaseSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(DatabaseSystem)
        void Init() override;
    };
} // base
