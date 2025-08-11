#include "MsgQueue.hpp"
#include "ProcStatusMgr.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/wait.h>
#include <csignal>
#include <vector>
#include <unistd.h>
#include <filesystem>

#define STRACE_FILTER_SCRIPT "strace_filter.py"

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

// Watch a process with strace, write output to file, and call Python script to process results
void watch_process_with_strace(pid_t target_pid, const std::string& strace_log, const std::string& python_script, const std::string& json_file) {
    std::cout << "Starting strace on PID " << target_pid << std::endl;


    // Build strace command
    std::string strace_cmd = "sudo timeout 5 strace -e trace=all -f -s 0 -yy -ttt -o " + strace_log + " -p " + std::to_string(target_pid);
    int ret = system(strace_cmd.c_str());
    if (ret != 0) {
        std::cerr << "strace failed or timed out for PID " << target_pid << std::endl;
        return;
    }

    // Build python command
    std::string python_cmd = "python3 " + python_script + " -s " + strace_log + " -j " + json_file;
    ret = system(python_cmd.c_str());
    if (ret != 0) {
        std::cerr << "Python script failed to process strace log." << std::endl;
        return;
    }

    std::cout << "Python script finished. Results updated in " << json_file << std::endl;
}

void ensure_log_path_exists(const std::string& log_path) {
    std::filesystem::path p(log_path);
    auto dir = p.parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
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
    auto full_path = std::filesystem::absolute(STRACE_FILTER_SCRIPT);

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
                ensure_log_path_exists(msg.get_log_file_path());
                watch_process_with_strace(msg.pid,
                                          STRACE_FILTER_SCRIPT,
                                          msg.get_log_file_path(),
                                          msg.get_json_file_path());
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