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

std::vector<std::tuple<int, int>> DBMgr::getDeadlockSuspicious(const std::string &db_name, int limit)
{
    std::vector<std::tuple<int, int>> results;
    std::vector<std::string> query_results;
    std::stringstream ss_query;
    
    // SQL query
    ss_query << "SELECT pid, ppid, etime, vsz, stat "
             << "FROM " << db_name << " "
             << "WHERE cpu = 0.0 "
             << "AND (stat LIKE 'D%' OR stat LIKE 'Z%' OR stat LIKE 'Sl%') "
             << "ORDER BY etime DESC, vsz DESC ";

    if (limit > 0)
    {
        ss_query << " LIMIT " << limit;
    }
    ss_query << ";" ;
    
    query_results = executeQuery(ss_query.str());
    
    for (auto &row : query_results)
    {
        std::vector<std::string> fields;
        boost::split(fields, row, boost::is_any_of("|"));
        for (auto &f : fields) boost::trim(f);

        if (fields.size() < 5) continue;

        int pid = std::stoi(fields[0]);
        int ppid = std::stoi(fields[1]);
        std::string etime_str = fields[2]; // format HH:MM or MM:SS
        int vsz = std::stoi(fields[3]);
        std::string stat = fields[4];

        // convert etime to minutes
        int minutes = 0;
        {
            std::vector<std::string> parts;
            boost::split(parts, etime_str, boost::is_any_of(":"));
            if (parts.size() == 2) // MM:SS
                minutes = std::stoi(parts[0]);
            else if (parts.size() == 3) // HH:MM:SS
                minutes = std::stoi(parts[0]) * 60 + std::stoi(parts[1]);
        }

        // filter stuck processes
        if ((vsz > 5000 || minutes >= 2) && (stat[0] == 'D' || stat[0] == 'Z' || stat.substr(0,2) == "Sl"))
        {
            results.push_back(std::tuple(pid, ppid));
        }
    }
    
    return results;
}