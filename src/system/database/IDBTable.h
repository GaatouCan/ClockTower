#pragma once

#include <mysqlx/xdevapi.h>

class IDBTable {
public:
    virtual ~IDBTable() = default;

    virtual void query(mysqlx::Schema &schema) = 0;
    virtual void read(mysqlx::Row &row) = 0;
    virtual void write(mysqlx::Schema &schema) = 0;
    virtual void remove(mysqlx::Schema &schema) = 0;
};
