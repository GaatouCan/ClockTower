#pragma once

#include "db_table.h"
#include "../../common.h"

#include <mysqlx/xdevapi.h>
#include <utility>


class USerializer final {
    mysqlx::Table mTable;

public:
    explicit USerializer(mysqlx::Table table)
        : mTable(std::move(table)) {
    }

    DISABLE_COPY_MOVE(USerializer)

    template<DB_TABLE_TYPE T, class... Args>
    void SerializeT(Args &&... args) {
        T table(std::forward<Args>(args)...);
        Serialize(table);
    }

    void Serialize(IDBTable *table) {
        table->Write(mTable);
    }
};
