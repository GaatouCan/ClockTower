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

        void PushTask(DBTask task);

        /**
         * 添加数据库读写任务和调用回调
         * @tparam Callback 无参无返回可调用类型
         * @param task 数据库任务
         * @param cb 回调闭包
         */
        template<typename Callback>
        void PushTask(DBTask task, Callback && cb) {
            PushTask([task = std::move(task), cb = std::forward<Callback>(cb)](mysqlx::Schema &schema) {
                std::invoke(task, schema);
                std::move(cb)();
            });
        };

        template<asio::completion_token_for<void()> Token>
        auto AsyncPushTask(DBTask task, Token && token) {
            auto init = [](asio::completion_handler_for<void()> auto handler, DBTask func) {
                auto work = asio::make_work_guard(handler);

                PushTask(std::move(func), [handler = std::move(handler), work = std::move(work)]() mutable {
                    auto alloc = asio::get_associated_allocator(handler, asio::recycling_allocator<void>());
                    asio::dispatch(work.get_executor(), asio::bind_allocator(alloc, [handler = std::move(handler)]() mutable {
                        std::move(handler)();
                    }));
                });
            };

            return asio::async_initiate<Token, void()>(init, token, std::move(task));
        }

    private:
        std::vector<DBSystemNode> nodeVec_;
        std::atomic_size_t nextNodeIdx_ = 0;
    };
} // base
