#pragma once

#include "DBTable.h"
#include "../../common/common.h"

#include <mysqlx/xdevapi.h>
#include <utility>


class USerializer final {
    mysqlx::Table table_;

public:
    explicit USerializer(mysqlx::Table table)
        : table_(std::move(table)) {
    }

    DISABLE_COPY_MOVE(USerializer)

    template<DBTABLE_TYPE T, class... Args>
    void SerializeT(Args &&... args) {
        T table(std::forward<Args>(args)...);
        Serialize(table);
    }

    void Serialize(IDBTable *table) {
        table->Write(table_);
    }
};
