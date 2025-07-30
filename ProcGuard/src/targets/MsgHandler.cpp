#include "MsgQueue.hpp"
#include "ProcStatusMgr.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/wait.h>
#include <csignal>
#include <vector>


std::vector<pid_t> children;

// Watch a single process for changes
void watch_process(pid_t pid) {
    int status;
    std::cout << "Watching process " << pid << std::endl;

    // Wait for the process to terminate
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        std::cout << "Process " << pid << " exited with code " << WEXITSTATUS(status) << std::endl;
    } else if (WIFSIGNALED(status)) {
        std::cout << "Process " << pid << " was killed by signal " << WTERMSIG(status) << std::endl;
    } else {
        std::cout << "Process " << pid << " ended unexpectedly." << std::endl;
    }
}


void signal_handler()
{
    for (pid_t pid : children)
    {
        kill(pid, SIGTERM);
    }
    exit(0);
}

int main()
{
    MsgQueue mq;
    ProcStatusMgr &shm = ProcStatusMgr::getInstance();

    std::cout << "MsgHandler started. Waiting for messages..." << std::endl;


    while (true)
    {
        Msg_t msg;
        if (mq.receive(msg) != -1)
        {
            std::cout << "Received message from PID: " << msg.pid
                      << " Type: " << (int)msg.type << std::endl;
            
            shm.setStatus(msg.pid, ProcStatus::CHECKING);

            pid_t pid = fork();
            if (pid == -1)
            {
                perror("fork failed");
            }
            else if (pid == 0)
            {
                watch_process(msg.pid);
                exit(0);
            }
            else
            {
                // parent
                children.push_back(pid);
                std::cout << "Started watcher process for PID " << msg.pid << " (watcher PID: " << pid << ")" << std::endl;

            }
        }
    }
    
    return 0;
}