// Suspect Process Memory Handler
#include "SPMemHandler.hpp"
#include <iostream>


SPMemHandler::SPMemHandler(int pid) : ISPHandler(pid) {}

SPMemHandler::~SPMemHandler() {}

int SPMemHandler::watch(int intervals, int time)
{
    int status;
    DStraceHandler strace_handler;

    for (int i = 0; i < intervals; i++)
    {
        std::cout << "[*] SPMemHandler: Collecting strace data for PID " << pid << " (interval " << (i + 1) << "/" << intervals << ")..." << std::endl;
        status = strace_handler.collect(pid_folder, pid, time);
        if (status != 0)
        {
            std::cerr << "[!] SPMemHandler: Error collecting strace data for PID " << pid << "!" << std::endl;
            break;
        }
    }

    return status;
}

int SPMemHandler::analyze()
{
    int status;
    DStraceHandler strace_handler;
    
    std::cout << "[*] SPMemHandler: Analyzing strace data for PID " << pid << "..." << std::endl;
    status = strace_handler.analyze(pid_folder, pid);
    
    return status;
}
