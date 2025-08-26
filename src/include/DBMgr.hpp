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
#define F_RSS    "rss"
#define F_stat   "stat"
#define F_flags  "flags"


class DBMgr
{
private:
    std::string db_path;
    std::mutex mtx;
    sqlite3 *db;

    std::vector<std::string> executeQuery(const std::string& query);
    std::vector<std::tuple<int, float>> getField(const std::string &db_name, const std::string &field, int limit);
public:
    /* 
     * @brief: Constructor to initialize DBMgr with database path.
     * @param db_path The path to the database file.
     * @note: This function should be called only once at the start of the program.
     */
    DBMgr(const std::string& db_path);
    ~DBMgr();

    std::vector<std::tuple<int, float>> getMaxCPU(const std::string &db_name, int limit = -1);
    std::vector<std::tuple<int, float>> getMaxMEM(const std::string &db_name, int limit = -1);
    std::vector<std::tuple<int, float>> getMaxRSS(const std::string &db_name, int limit = -1);

};