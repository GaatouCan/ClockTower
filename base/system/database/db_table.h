#pragma once

#include "../../byte_array.h"

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
concept DB_TABLE_TYPE = std::derived_from<T, IDBTable>;

#define DB_CAST_FROM_BLOB(field, value) \
{ \
    const auto bytes = value.getRawBytes(); \
    field = FByteArray(std::vector(bytes.begin(), bytes.end())); \
}

#define DB_CAST_TO_BLOB(field) \
mysqlx::bytes(field.data(), field.size())
