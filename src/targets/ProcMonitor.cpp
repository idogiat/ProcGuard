#include "DBMgr.hpp"
#include "ProcStatusMgr.hpp"
#include "ProcBlackList.hpp"
#include "MsgQueue.hpp"
#include "common.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <signal.h>

#define MAX_PS  8

static void send_msg(const std::vector<std::tuple<int, float>> &result, ProcStatusMgr &ps_mgr, ProcBlackList &ps_bl);

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
        send_msg(db->getMaxCPU(DB_NAME, MAX_PS), ps_mgr, ps_bl);
        send_msg(db->getMaxMEM(DB_NAME, MAX_PS), ps_mgr, ps_bl);
        send_msg(db->getMaxRSS(DB_NAME, MAX_PS), ps_mgr, ps_bl);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    mq->remove();
    delete db;
    delete mq;
    return 0;
}


static void send_msg(const std::vector<std::tuple<int, float>> &result, ProcStatusMgr &ps_mgr, ProcBlackList &ps_bl)
{
    for (auto r : result)
        {
            int pid = std::get<0>(r);
            if (!ps_bl.isExists(pid))
            {
                if(ps_mgr.addPid(pid) != -1)
                {
                    Msg_t m = {pid, ProcType::CPU};
                    mq->send(m);
                    std::cout << "ID: " << pid << std::endl;
                }
            }
        }
}