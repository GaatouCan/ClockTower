#pragma once

#include "IDBTable.h"
#include "../../common/common.h"

#include <mysqlx/xdevapi.h>

namespace base {
    class Serializer final {
        mysqlx::Schema &schema_;

    public:
        explicit Serializer(mysqlx::Schema &schema)
            : schema_(schema) {
        }

        DISABLE_COPY_MOVE(Serializer)

        template<DBTABLE_TYPE T, class ... Args>
        void SerializeT(Args &&... args) {
            T table(std::forward<Args>(args)...);
            Serialize(table);
        }

        void Serialize(IDBTable *table) const {
            table->Write(schema_);
        }
    };
}
