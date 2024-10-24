#include "DatabaseSystem.h"
#include "../config/ConfigSystem.h"
#include "../../common/utils.h"

#include <spdlog/spdlog.h>

namespace base {
    void DatabaseSystem::Init() {
        const auto &cfg = GetServerConfig();
        const auto schemaName = cfg["database"]["mysql"]["schema"].as<std::string>();

        nodeVec_ = std::vector<DBSystemNode>(cfg["database"]["pool"].as<uint64_t>());

        for (auto & node : nodeVec_) {
            node.sess = std::make_unique<mysqlx::Session>(
                cfg["database"]["mysql"]["host"].as<std::string>(),
                cfg["database"]["mysql"]["port"].as<uint16_t>(),
                cfg["database"]["mysql"]["user"].as<std::string>(),
                cfg["database"]["mysql"]["passwd"].as<std::string>()
            );
            node.queue = std::make_unique<TSDeque<IDBCallbackWrapper *>>();

            node.th = std::make_unique<std::thread>([this, &node, &schemaName] {
                node.tid = std::this_thread::get_id();
                spdlog::info("Thread ID {} - Begin handle database task", ThreadIdToInt(node.tid));
                while (node.queue->IsRunning()) {
                    node.queue->Wait();
                    if (!node.queue->IsRunning())
                        break;

                    if (const auto wrapper = node.queue->PopFront(); wrapper != nullptr) {
                        if (auto schema = node.sess->getSchema(schemaName); schema.existsInDatabase()) {
                            wrapper->Execute(schema);
                        }
                        delete wrapper;
                    }
                }

                node.queue->Clear();
                node.sess->close();
            });
        }
    }

    DatabaseSystem::~DatabaseSystem() {
        for (const auto &[th, sess, queue, tid] : nodeVec_) {
            if (th->joinable()) {
                th->join();
            }
        }

        for (const auto &[th, sess, queue, tid] : nodeVec_) {
            queue->Quit();
        }
    }

    void DatabaseSystem::SyncSelect(const std::string &tableName, const std::string &where, const std::function<void(mysqlx::Row)> &cb) {
        if (nodeVec_.empty()) {
            spdlog::error("{} - {}", __func__, __LINE__);
            return;
        }

        const auto &[th, sess, queue, tid] = nodeVec_.front();
        const auto &cfg = GetServerConfig();

        auto schema = sess->getSchema(cfg["database"]["mysql"]["schema"].as<std::string>());
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

    void DatabaseSystem::PushTask(const DBTask &task) {
        PushTask(task, []{});
    }
} // base