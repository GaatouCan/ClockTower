/**
 * Object Relational Mapping CPP Class Define
 * This file is generated by Python script. Do not edit!!!
 * Generated time: 2024-10-18 19:58:20
 * Python version: v3.12.7
 * Script version: v0.1
 * Source file: /sql/appearance
 */

#pragma once

#include "../../src/system/database/IDBTable.h"

namespace orm {

	class DBTable_Appearance final : public IDBTable {
	public:
		uint64_t pid = 0;
		uint32_t avatar = 0;
		uint32_t avatar_frame = 0;

		DBTable_Appearance(
			uint64_t pid, 
			uint32_t avatar, 
			uint32_t avatar_frame
		);

		mysqlx::RowResult Query(mysqlx::Schema &schema) override;
		void Read(mysqlx::Row &row) override;
		void Write(mysqlx::Schema &schema) override;
		void Remove(mysqlx::Schema &schema) override;
	}; // DBTable_Appearance

	class DBTable_Avatar final : public IDBTable {
	public:
		uint64_t pid = 0;
		uint32_t index = 0;
		uint64_t expired_time = 0;
		int16_t activated = 0;
		int16_t in_used = 0;

		DBTable_Avatar(
			uint64_t pid, 
			uint32_t index, 
			uint64_t expired_time, 
			int16_t activated, 
			int16_t in_used
		);

		mysqlx::RowResult Query(mysqlx::Schema &schema) override;
		void Read(mysqlx::Row &row) override;
		void Write(mysqlx::Schema &schema) override;
		void Remove(mysqlx::Schema &schema) override;
	}; // DBTable_Avatar

	class DBTable_AvatarFrame final : public IDBTable {
	public:
		uint64_t pid = 0;
		uint32_t index = 0;
		uint64_t expired_time = 0;
		int16_t activated = 0;
		int16_t in_used = 0;

		DBTable_AvatarFrame(
			uint64_t pid, 
			uint32_t index, 
			uint64_t expired_time, 
			int16_t activated, 
			int16_t in_used
		);

		mysqlx::RowResult Query(mysqlx::Schema &schema) override;
		void Read(mysqlx::Row &row) override;
		void Write(mysqlx::Schema &schema) override;
		void Remove(mysqlx::Schema &schema) override;
	}; // DBTable_AvatarFrame

} // orm