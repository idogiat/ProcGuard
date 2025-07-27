#pragma once
#include <sqlite3.h>
#include <string>
#include <sstream>
#include <vector>
#include <mutex>
#include <iostream>
#include <tuple>

#define DB_PATH "ps_snapshot.db"
#define DB_NAME "snapshots"

#define F_P_ID   "pid"
#define F_PP_ID  "ppid"
#define F_comm   "comm"
#define F_CPU    "cpu"
#define F_MEMORY "mem"
#define F_stat   "stat"
#define F_flags   "flags"


class DBManager
{
private:
    sqlite3 *db;
    std::string db_path;
    std::mutex mtx;

    std::vector<std::string> executeQuery(const std::string& query);
    std::vector<std::tuple<int, float>> getField(const std::string &db_name, const std::string &field, int limit);
public:
    DBManager(const std::string& db_path);
    ~DBManager();

    std::vector<std::tuple<int, float>> getMaxCPU(const std::string &db_name, int limit = -1);
    std::vector<std::tuple<int, float>> getMaxMEM(const std::string &db_name, int limit = -1);

};