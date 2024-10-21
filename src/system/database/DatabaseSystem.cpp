#include "DatabaseSystem.h"
#include "../config/ConfigSystem.h"

#include <spdlog/spdlog.h>

namespace base {
    void DatabaseSystem::Init() {
    }

    void DatabaseSystem::SyncSelect(const std::string &tableName, const std::string &where, const std::function<void(mysqlx::Row)> &cb) {
        if (nodeVec_.empty()) {
            spdlog::error("{} - {}", __func__, __LINE__);
        }

        auto &[th, tid, sess, queue] = nodeVec_.front();
        const auto &cfg = GetServerConfig();

        auto schema = sess.getSchema(cfg["database"]["mysql"]["schema"].as<std::string>());
        if (!schema.existsInDatabase()) {
            spdlog::error("{} - schema not exists", __func__);
            return;
        }

        auto table = schema.getTable(tableName);
        if (!table.existsInDatabase()) {
            spdlog::error("{} - table not exists", __func__);
            return;
        }

        for (auto res = table.select().where(where).execute(); const auto &row : res) {
            cb(row);
        }
    }
} // base