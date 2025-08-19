#include "MsgQueue.hpp"
#include "ProcStatusMgr.hpp"
#include "ProcBlackList.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <sys/wait.h>
#include <csignal>
#include <vector>
#include <unistd.h>
#include <filesystem>


#define PARSER_SCRIPT   "tools/strace_parser.py"
#define ANALYZER_SCRIPT "tools/proc_analyzer.py"
#define DATA_FILE       "ml_data/data.json"


static int watch_process_with_strace(pid_t target_pid,
                                     const std::string& parser_script,
                                     const std::string& strace_log,
                                     const std::string& json_file);
static void ensure_log_path_exists(const std::string& log_path);

static std::vector<pid_t> children;
static ProcBlackList &ps_bl = ProcBlackList::getInstance();
static constexpr int max_iterations = 5; // Number of iterations to run strace
static constexpr int wait_seconds = 1; // Maximum wait time in seconds for strace to finish

static void signal_handler(int signum)
{
    (void)signum;
    for (pid_t pid : children)
    {
        kill(pid, SIGTERM);
        ps_bl.removePid(pid);
    }
    exit(0);
}


int main()
{
    std::signal(SIGINT, signal_handler);
    MsgQueue mq;
    ProcStatusMgr &shm = ProcStatusMgr::getInstance();
    auto parser_script = std::filesystem::absolute(PARSER_SCRIPT);

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
                int ret = watch_process_with_strace(msg.pid,
                                                    parser_script.string(),
                                                    msg.get_log_file_path(),
                                                    msg.get_json_file_path());

                switch (ret)
                {
                case 0:
                    std::cout << "The process id " << msg.pid << " is ok" << std::endl;
                    shm.setStatus(msg.pid, ProcStatus::OK);
                    break;
                
                case 1:
                    std::cout << "The process id " << msg.pid << " is suspicious" << std::endl;
                    shm.setStatus(msg.pid, ProcStatus::SUSPICIOUS);
                    break;
                case -1:
                    std::cout << "The process id " << msg.pid << " is not ok" << std::endl;
                    shm.setStatus(msg.pid, ProcStatus::ANALYZE_ERROR);
                    break;
                default:
                    break;
                }

                // Clean up shared memory and blacklist
                pid_t current_pid = getpid();
                ps_bl.removePid(current_pid);
                children.erase(std::remove(children.begin(), children.end(), current_pid), children.end());               
                exit(0);
            }
            else
            {
                // parent
                children.push_back(pid);
                ps_bl.addPid(pid);
                std::cout << "Started watcher process for PID " << msg.pid << " (watcher PID: " << pid << ")" << std::endl;
            }
        }
    }
    
    return 0;
}

/* Watch a process with strace, write output to file, and call Python script to process results
 * @param target_pid The PID of the process to watch
 * @param parser_script The path to the Python script that processes strace output
 * @param strace_log The path to the file where strace output will be written
 * @param json_file The path to the JSON file where results will be saved
 * @return 0 on normal process, -1 on failure
*/
static int watch_process_with_strace(pid_t target_pid,
                                      const std::string& parser_script,
                                      const std::string& strace_log,
                                      const std::string& json_file)
{
    int ret;
    std::cout << "Starting strace on PID " << target_pid << std::endl;

    // Build commands
    std::string strace_cmd = "sudo timeout 5s strace -e trace=all -f -s 0 -yy -ttt -o " + strace_log +
                             " -p " + std::to_string(target_pid);
    std::string parser_cmd = "python3 " + parser_script + 
                                " -s " + strace_log +
                                " -j " + json_file;
    std::string analyzer_cmd = "python3 " + std::string(ANALYZER_SCRIPT) +
                                " -j " + json_file +
                                " -d " +  std::string(DATA_FILE) +
                                " -p " + std::to_string(target_pid);
    
    for (int iteration = 1; iteration <= max_iterations; iteration++)
    {
        ret = system(strace_cmd.c_str());

        // Build python command
        ret = system(parser_cmd.c_str());
        if (ret != 0) {
            std::cerr << "Python script failed to process strace log." << std::endl;
            return -1;
        }

        std::cout << "Python script finished. Results updated in " << json_file << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(wait_seconds));
    }

    ret = system(analyzer_cmd.c_str());

    int exit_code = WEXITSTATUS(ret);
    return exit_code;
}

static void ensure_log_path_exists(const std::string& log_path)
{
    std::filesystem::path p(log_path);
    auto dir = p.parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}
