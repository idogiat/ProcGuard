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
            if(ps_mgr.addPid(pid) != 0)
            {

            }
        }
        auto result2 = db->getMaxMEM(DB_NAME, MAX_THREADS);
        for (auto r : result2)
        {
            int pid = std::get<0>(r);
            std::cout << "ID: " << pid << " MEM: " << std::get<1>(r) << std::endl;
            if(ps_mgr.addPid(pid) != 0)
            {
                
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    delete db;
    return 0;
}