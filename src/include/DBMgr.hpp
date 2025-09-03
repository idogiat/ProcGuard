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

    /* 
     * @brief: Execute a SQL query and return the results as a vector of strings.
     * @param query The SQL query to execute.
     * @retval: A vector of strings, each representing a row in the result set.
     * @note: This function is thread-safe.
     */
    std::vector<std::string> executeQuery(const std::string& query);
    
    /* 
     * @brief: Get a list of PIDs and their corresponding field values.
     * @param db_name The name of the database table to query.
     * @param field The field to retrieve (e.g., "cpu", "mem", "rss").
     * @param limit The maximum number of results to return. If -1, return all results.
     * @retval: A vector of tuples, each containing the PID and the requested field value.
     * @note: This function should be called only once at the start of the program.
     */
    std::vector<std::tuple<int, float>> getField(const std::string &db_name, const std::string &field, int limit);
public:
    /* 
     * @brief: Constructor to initialize DBMgr with database path.
     * @param db_path The path to the database file.
     * @note: This function should be called only once at the start of the program.
     */
    DBMgr(const std::string& db_path);
    ~DBMgr();

    /* 
     * @brief: Get a list of PIDs and their corresponding CPU usage.
     * @param db_name The name of the database table to query.
     * @param limit The maximum number of results to return. If -1, return all results.
     * @retval: A vector of tuples, each containing the PID and the requested field value.
     * @note: This function should be called only once at the start of the program.
     */
    std::vector<std::tuple<int, float>> getMaxCPU(const std::string &db_name, int limit = -1);
    
    /* 
     * @brief: Get a list of PIDs and their corresponding memory usage.
     * @param db_name The name of the database table to query.
     * @param limit The maximum number of results to return. If -1, return all results.
     * @retval: A vector of tuples, each containing the PID and the memory usage value.
     * @note: This function should be called only once at the start of the program.
     */
    std::vector<std::tuple<int, float>> getMaxMEM(const std::string &db_name, int limit = -1);

    /* 
     * @brief: Get a list of PIDs and their corresponding RSS.
     * @param db_name The name of the database table to query.
     * @param limit The maximum number of results to return. If -1, return all results.
     * @retval: A vector of tuples, each containing the PID and the RSS value.
     * @note: This function should be called only once at the start of the program.
     */
    std::vector<std::tuple<int, float>> getMaxRSS(const std::string &db_name, int limit = -1);

    /* 
     * @brief: Get a list of PIDs and PPIDs of processes that are potentially deadlocked or stuck.
     * @param db_name The name of the database table to query.
     * @param limit The maximum number of results to return. If -1, return all results.
     * @retval: A vector of tuples, each containing the PID and PPID of a suspicious process.
     * @note: This function should be called only once at the start of the program.
     */
    std::vector<std::tuple<int, int>> getDeadlockSuspicious(const std::string &db_name, int limit = -1);
};
