#include "ProcStatusMgr.hpp"
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>

#define SLEEP_INTERVAL 5 // seconds

bool run = true;


void signal_handler(int signum)
{
    std::cout << "Signal (" << signum << ") received, exiting..." << std::endl;
    run = false;
}


int main()
{
    std::signal(SIGINT, signal_handler);
    ProcStatusMgr &shm = ProcStatusMgr::getInstance();

    while (run)
    {
        std::vector<PidEntry> pids = shm.getAllPIDs();
        
        for (const auto &pid_entry : pids)
        {
            if (kill(pid_entry.pid, 0) == -1 && errno == ESRCH)
            {
                std::cout << "Process " << pid_entry.pid << " does not exist. Removing from shared memory." << std::endl;
                shm.removePid(pid_entry.pid);
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(SLEEP_INTERVAL));
    }
    
}