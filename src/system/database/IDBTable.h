#pragma once

#include <mysqlx/xdevapi.h>

class IDBTable {
public:
    virtual ~IDBTable() = default;

    virtual std::string GetTableName() = 0;

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
