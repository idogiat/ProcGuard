#include "DBMgr.hpp"
#include <boost/algorithm/string.hpp>

DBMgr::DBMgr(const std::string& db_path): db_path(db_path), db(nullptr)
{
    std::lock_guard<std::mutex> lock(mtx);
    if(sqlite3_open(db_path.c_str(), &db) != SQLITE_OK)
    {
        std::cerr << "Cannot open database" << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

DBMgr::~DBMgr()
{
    if (db)
    {
        sqlite3_close(db);
    }
}


std::vector<std::string> DBMgr::executeQuery(const std::string& query)
{
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> results;
    sqlite3_stmt* stmt = nullptr;

    // make query
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return results;
    }

    // exec query and parse results
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        std::stringstream ss;
        int cols = sqlite3_column_count(stmt);
        for (int i = 0; i < cols; ++i)
        {
            const unsigned char* text = sqlite3_column_text(stmt, i);
            ss << (text ? reinterpret_cast<const char*>(text) : "NULL");
            if (i < cols - 1)
            {
                ss << " | ";
            }
        }
        results.push_back(ss.str());
    }

    // cleanup
    sqlite3_finalize(stmt);
    return results;
}


std::vector<std::tuple<int, float>> DBMgr::getField(const std::string &db_name, const std::string &field, int limit)
{
    std::vector<std::tuple<int, float>> results;
    std::vector<std::string> general_results;
    std::stringstream ss_query;
    
    ss_query << "SELECT " << F_P_ID << ", " <<  field
             << " FROM " << db_name
             << " ORDER BY " << field << " DESC";

             if (limit > 0)
    {
        ss_query << " LIMIT " << limit;
    }
    ss_query << ";" ;
    
    general_results = executeQuery(ss_query.str());
    
    std::string part1, part2;
    for (auto r : general_results)
    {
        std::vector<std::string> v;
        boost::split(v, r, boost::is_any_of("|"));

        for (auto &s : v)
        {
            boost::trim(s);
        }
        
        if (!v.empty())
        {
            results.push_back(std::tuple(std::stoi(v.at(0)), std::stof(v.at(1))));
        }
    }
    
    return results;
}

std::vector<std::tuple<int, float>> DBMgr::getMaxCPU(const std::string &db_name, int limit)
{
    return getField(db_name, F_CPU, limit);
}

std::vector<std::tuple<int, float>> DBMgr::getMaxMEM(const std::string &db_name, int limit)
{
    return getField(db_name, F_MEMORY, limit);

}

std::vector<std::tuple<int, float>> DBMgr::getMaxRSS(const std::string &db_name, int limit)
{
    return getField(db_name, F_RSS, limit);
}