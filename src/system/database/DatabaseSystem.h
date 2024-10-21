#pragma once

#include "../../base/ISubSystem.h"
#include "../../base/TSDeque.h"

#include <thread>
#include <vector>
#include <mysqlx/xdevapi.h>

namespace base {

    using DBTask = std::function<void(mysqlx::Schema &)>;

    struct DBSystemNode {
        std::unique_ptr<std::thread> th;
        std::unique_ptr<mysqlx::Session> sess;
        std::unique_ptr<TSDeque<DBTask>> queue;
        ThreadID tid;
    };

    class DatabaseSystem final : public ISubSystem {
        SUB_SYSTEM_BODY(DatabaseSystem)
        void Init() override;

        ~DatabaseSystem() override;

    public:
        /**
         * 阻塞SQL查询，只能用于服务初始化时数据加载
         * @param tableName 表名
         * @param where 条件语句
         * @param cb 回调函数
         */
        void SyncSelect(const std::string &tableName, const std::string &where, const std::function<void(mysqlx::Row)> &cb);

    private:
        std::vector<DBSystemNode> nodeVec_;
        std::atomic_size_t nextNodeIdx_ = 0;
    };
} // base
