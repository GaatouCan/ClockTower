#pragma once

#include "../../ByteArray.h"

#include <mysqlx/xdevapi.h>

class IDBTable {
public:
    virtual ~IDBTable() = default;

    [[nodiscard]] virtual const char*  GetTableName() const = 0;

    virtual mysqlx::RowResult Query(mysqlx::Table &table) = 0;
    virtual mysqlx::RowResult Query(mysqlx::Schema &schema) = 0;

    virtual void Read(mysqlx::Row &row) = 0;

    virtual void Write(mysqlx::Table &table) = 0;
    virtual void Write(mysqlx::Schema &schema) = 0;

    virtual void Remove(mysqlx::Table &table) = 0;
    virtual void Remove(mysqlx::Schema &schema) = 0;
};

template <typename T>
concept DBTABLE_TYPE = std::derived_from<T, IDBTable>;

#define DB_CAST_FROM_BLOB(field, value) \
{ \
    const auto bytes = value.getRawBytes(); \
    field = std::vector(bytes.begin(), bytes.end()); \
}

#define DB_CAST_TO_BLOB(field) \
mysqlx::bytes(field.GetRawPointer(), field.GetLength())
