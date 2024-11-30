#pragma once

#include "../../SubSystem.h"
#include "../../TSDeque.h"
#include "../../DatabaseWrapper.h"

#include <thread>
#include <vector>
#include <utility>

struct FDatabaseNode {
    std::unique_ptr<std::thread> th;
    std::unique_ptr<mysqlx::Session> sess;
    std::unique_ptr<TSDeque<IDatabaseWrapper *> > queue;
    AThreadID tid;
};

class UDatabaseSystem final : public ISubSystem {

    std::vector<FDatabaseNode> mNodeList;
    std::atomic_size_t mNextNodeIndex = 0;

public:
    ~UDatabaseSystem() override;

    void Init() override;

    [[nodiscard]] constexpr const char *GetSystemName() const override { return "UDatabaseSystem"; }

    /**
     * 阻塞SQL查询，只能用于服务初始化时数据加载
     * @param tableName 表名
     * @param where 条件语句
     * @param cb 回调函数
     */
    void BlockSelect(const std::string &tableName, const std::string &where, const std::function<void(mysqlx::Row)> &cb);

    void PushTask(const ADatabaseTask &task);

    template<typename Callback>
    void PushTask(const ADatabaseTask &task, Callback &&cb) {
        const auto &[th, sess, queue, tid] = mNodeList[mNextNodeIndex++];
        mNextNodeIndex = mNextNodeIndex % mNodeList.size();
        queue->PushBack(new UDBTaskWrapper(task, std::forward<Callback>(cb)));
    }

    template<asio::completion_token_for<void(bool)> CompletionToken>
    auto AsyncTask(const ADatabaseTask &task, CompletionToken &&token) {
        auto init = [this](asio::completion_handler_for<void(bool)> auto handler, const ADatabaseTask &func) {
            auto work = asio::make_work_guard(handler);

            PushTask(func, [handler = std::move(handler), work = std::move(work)](bool ret) mutable {
                auto alloc = asio::get_associated_allocator(handler, asio::recycling_allocator<void>());
                asio::dispatch(work.get_executor(), asio::bind_allocator(alloc, [handler = std::move(handler), ret]() mutable {
                    std::move(handler)(ret);
                }));
            });
        };

        return asio::async_initiate<CompletionToken, void(bool)>(init, token, task);
    }

    /**
     * 添加查找任务并在查找完成时调用回调函数
     * @tparam Callback 回调函数类型 void(bool ret)
     * @param table 目标查找表名
     * @param where 查找条件
     * @param cb 回调函数
     */
    template<typename Callback>
    void PushSelectTask(const std::string &table, const std::string &where, Callback &&cb) {
        const auto &[th, sess, queue, tid] = mNodeList[mNextNodeIndex++];
        mNextNodeIndex = mNextNodeIndex % mNodeList.size();

        queue->PushBack(new TDBSelectWrapper<Callback>(table, where, std::forward<Callback>(cb)));
    }

    /**
     * asio协程版本异步查找
     * @tparam CompletionToken 完成令牌类型
     * @param table 目标查找表名
     * @param where 查找条件
     * @param token 完成令牌
     * @return mysqlx::RowResult的完成令牌包装器
     */
    template<asio::completion_token_for<void(std::optional<mysqlx::RowResult>)> CompletionToken>
    auto AsyncSelect(const std::string &table, const std::string &where, CompletionToken &&token) {
        auto init = [this](asio::completion_handler_for<void(std::optional<mysqlx::RowResult>)> auto handler, const std::string &tableName, const std::string &whereExpression) {
            auto work = asio::make_work_guard(handler);

            PushSelectTask(tableName, whereExpression, [handler = std::move(handler), work = std::move(work)](std::optional<mysqlx::RowResult> result) mutable {
                auto alloc = asio::get_associated_allocator(handler, asio::recycling_allocator<void>());
                asio::dispatch(work.get_executor(), asio::bind_allocator(alloc, [handler = std::move(handler), result = std::forward<std::optional<mysqlx::RowResult>>(result)]() mutable {
                    std::move(handler)(std::move(result));
                }));
            });
        };

        return asio::async_initiate<CompletionToken, void(std::optional<mysqlx::RowResult>)>(init, token, table, where);
    }
};
