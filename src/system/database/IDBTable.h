#pragma once

#include <mysqlx/xdevapi.h>

class IDBTable {
public:
    virtual ~IDBTable() = default;

    virtual void Query(mysqlx::Schema &schema) = 0;
    virtual void Read(mysqlx::Row &row) = 0;
    virtual void Write(mysqlx::Schema &schema) = 0;
    virtual void Remove(mysqlx::Schema &schema) = 0;
};
