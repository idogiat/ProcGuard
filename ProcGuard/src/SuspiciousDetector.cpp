#include "DBMgr.hpp"
#include "ProcStatusMgr.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <signal.h>

#define MAX_THREADS 5

static bool running = true;

static void handle_signal(int signum)
{
    running = false;
}


int main()
{
    signal(SIGINT, handle_signal);
    DBMgr *db = new DBMgr(DB_PATH);
    ProcStatusMgr &ps_mgr = ProcStatusMgr::getInstance();

    while (running)
    {
        auto result1 = db->getMaxCPU(DB_NAME, MAX_THREADS);
        for (auto r : result1)
        {
            int pid = std::get<0>(r);
            std::cout << "ID: " << pid  << " CPU: " << std::get<1>(r) << std::endl;
            ps_mgr.setStatus(pid, ProcStatus::WAITING);
        }
        auto result2 = db->getMaxMEM(DB_NAME, MAX_THREADS);
        for (auto r : result2)
        {
            int pid = std::get<0>(r);
            std::cout << "ID: " << pid << " MEM: " << std::get<1>(r) << std::endl;
            ps_mgr.setStatus(pid, ProcStatus::WAITING);
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    delete db;
    return 0;
}