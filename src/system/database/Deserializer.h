#pragma once

#include <mysqlx/xdevapi.h>

#include "IDBTable.h"
#include "../../common/common.h"

namespace base {
    class Deserializer final {

        mysqlx::RowResult result_;
        mysqlx::Row curRow_;

    public:
        explicit Deserializer(mysqlx::RowResult res)
            : result_(std::move(res)) {}

        bool HasMore() {
            curRow_ = result_.fetchOne();
            return !curRow_.isNull();
        }

        template<DBTABLE_TYPE T>
        T TDeserialize() {
            T res;
            Deserialize(&res);
            return res;
        }

        void Deserialize(IDBTable *row) {
            if (curRow_.isNull())
                curRow_ = result_.fetchOne();

            if (curRow_.isNull())
                return;

            row->Read(curRow_);
        }
    };
}