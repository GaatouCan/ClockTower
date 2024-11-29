#pragma once

#include "../../SubSystem.h"
#include "../../TSDeque.h"
#include "../../DBCallbackWrapper.h"

#include <thread>
#include <vector>
#include <utility>


struct FDatabaseNode {
    std::unique_ptr<std::thread> th;
    std::unique_ptr<mysqlx::Session> sess;
    std::unique_ptr<TSDeque<IDBCallbackWrapper *> > queue;
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
    void SyncSelect(const std::string &tableName, const std::string &where, const std::function<void(mysqlx::Row)> &cb);

    void PushTask(const ADatabaseTask &task);

    template<typename Callback>
    void PushTask(const ADatabaseTask &task, Callback &&cb) {
        const auto &[th, sess, queue, tid] = mNodeList[mNextNodeIndex++];
        mNextNodeIndex = mNextNodeIndex % mNodeList.size();

        queue->PushBack(new TDBCallbackWrapper<Callback>(task, std::forward<Callback>(cb)));
    }

    template<asio::completion_token_for<void(std::shared_ptr<mysqlx::RowResult>)> CompletionToken>
    auto AsyncPushTask(const ADatabaseTask &task, CompletionToken &&token) {
        auto init = [this](asio::completion_handler_for<void(std::shared_ptr<mysqlx::RowResult>)> auto handler, const ADatabaseTask &func) {
            auto work = asio::make_work_guard(handler);

            PushTask(func, [handler = std::move(handler), work = std::move(work)](std::shared_ptr<mysqlx::RowResult> result) mutable {
                auto alloc = asio::get_associated_allocator(handler, asio::recycling_allocator<void>());
                asio::dispatch(work.get_executor(), asio::bind_allocator(alloc,[handler = std::move(handler), result]() mutable {
                    std::move(handler)(result);
                }));
            });
        };

        return asio::async_initiate<CompletionToken, void(std::shared_ptr<mysqlx::RowResult>)>(init, token, task);
    }
};
