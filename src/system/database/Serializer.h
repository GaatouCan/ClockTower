#pragma once

#include "IDBTable.h"
#include "../../common/common.h"

#include <mysqlx/xdevapi.h>

#include <utility>

namespace base {

    class Serializer final {
        mysqlx::Table table_;

    public:
        explicit Serializer(mysqlx::Table table)
            : table_(std::move(table)) {
        }

        DISABLE_COPY_MOVE(Serializer)

        template<DBTABLE_TYPE T, class ... Args>
        void SerializeT(Args &&... args) {
            T table(std::forward<Args>(args)...);
            Serialize(table);
        }

        void Serialize(IDBTable *table) {
            table->Write(table_);
        }
    };
}
