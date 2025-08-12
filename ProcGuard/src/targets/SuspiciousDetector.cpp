#include "DBMgr.hpp"
#include "ProcStatusMgr.hpp"
#include "ProcBlackList.hpp"
#include "MsgQueue.hpp"
#include "common.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <signal.h>

#define MAX_THREADS 5

static bool running = true;
static MsgQueue *mq;

static void handle_signal(int signum)
{
    (void)signum;
    running = false; 
}


int main()
{
    signal(SIGINT, handle_signal);
    DBMgr *db = new DBMgr(DB_PATH);
    ProcStatusMgr &ps_mgr = ProcStatusMgr::getInstance();
    ProcBlackList &ps_bl = ProcBlackList::getInstance();
    mq = new MsgQueue();

    while (running)
    {
        auto result1 = db->getMaxCPU(DB_NAME, MAX_THREADS);
        for (auto r : result1)
        {
            int pid = std::get<0>(r);
            if (!ps_bl.isExists(pid))
            {
                if(ps_mgr.addPid(pid) != -1)
                {
                    Msg_t m = {pid, ProcType::CPU};
                    mq->send(m);
                    std::cout << "ID: " << pid  << " CPU: " << std::get<1>(r) << std::endl;
                }
            }
        }
        auto result2 = db->getMaxMEM(DB_NAME, MAX_THREADS);
        for (auto r : result2)
        {
            int pid = std::get<0>(r);
            if (!ps_bl.isExists(pid))
            {
                // Check if the process is already being monitored
                if(ps_mgr.addPid(pid) != -1)
                {
                    Msg_t m = {pid, ProcType::MEMORY};
                    mq->send(m);
                    std::cout << "ID: " << pid << " MEM: " << std::get<1>(r) << std::endl;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    mq->remove();
    delete db;
    delete mq;
    return 0;
}