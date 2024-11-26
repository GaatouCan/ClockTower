import pymysql
import os
import json

# 源数据库名称
SOURCE_DATABASE = "demo"

# 临时数据库名称
TARGET_DATABASE = SOURCE_DATABASE + "_temp"

# 源数据库连接信息
source_db_config = {
    "host": "192.168.50.172",
    "port": 3306,
    "user": "root",
    "password": "12345678",
    "database": SOURCE_DATABASE
}

table_list = []

try:
    # 连接到源数据库
    source_conn = pymysql.connect(**source_db_config)
    source_cursor = source_conn.cursor()

    # 获取源数据库所有表
    source_cursor.execute("SHOW TABLES")
    source_table_list = source_cursor.fetchall()

    # 连接到目标数据库服务器
    target_conn = pymysql.connect(
        host=source_db_config["host"],
        port=source_db_config["port"],
        user=source_db_config["user"],
        password=source_db_config["password"]
    )
    target_cursor = target_conn.cursor()

    # 创建临时数据库
    target_cursor.execute(f"CREATE DATABASE IF NOT EXISTS {TARGET_DATABASE}")
    print(f"正在创建临时数据库 `{TARGET_DATABASE}` ...")

    for (name,) in source_table_list:
        # 获取每个表的创建语句
        source_cursor.execute(f"SHOW CREATE TABLE {name}")
        create_table_sql = source_cursor.fetchone()[1]
            
        # 在临时数据库中创建表
        target_cursor.execute(f"USE {TARGET_DATABASE}")
        target_cursor.execute(create_table_sql)
        print(f"表 `{name}` 已复制到临时数据库")

        source_cursor.execute(f'DESCRIBE {name}')
        desc = source_cursor.fetchall()

        info = {
            "name": name, 
            "field": {},
            "key": []
        }
        for column in desc:
            field_name = column[0]

            info["field"][field_name] = {
                "name": column[0],
                "type": column[1],
                "null": column[2],
                "key": column[3],
                "default": column[4],
            }

            # 修饰一下数据
            info["field"][field_name]["null"] = column[2] == "YES"

            if column[3] != "":
                info["key"].append(field_name)


        table_list.append(info)

    target_conn.commit()
    print(f"临时数据库 `{TARGET_DATABASE}` 创建成功")

    target_cursor.execute(f'DROP DATABASE {TARGET_DATABASE}')
    target_conn.commit()
    print(f"临时数据库 `{TARGET_DATABASE}` 已删除")
    
except pymysql.MySQLError as e:
    print(f"数据库操作错误：{e}")
finally:
    # 关闭连接
    if source_conn:
        source_cursor.close()
        source_conn.close()
    if target_conn:
        target_cursor.close()
        target_conn.close()

# 生成JSON数据文件
with open('table_info.json', 'w', encoding='utf-8') as file:
    file.write(json.dumps(table_list, indent=4, ensure_ascii=False))
