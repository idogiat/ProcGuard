#include "ProcUtils.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>

std::string getProcessName(pid_t pid)
{
    std::string path = "/proc/" + std::to_string(pid) + "/comm";
    std::ifstream file(path);
    std::string name;
    if (file.is_open()) {
        std::getline(file, name);
    }
    return name;
}

std::string getProcessOwner(pid_t pid)
{
    std::string path = "/proc/" + std::to_string(pid);
    struct stat info{};
    if (stat(path.c_str(), &info) == 0) {
        struct passwd *pw = getpwuid(info.st_uid);
        if (pw) return pw->pw_name;
    }
    return "unknown";
}


int countChildren(pid_t pid)
{
    int count = 0;
    DIR *dir = opendir("/proc");
    if (!dir) return -1;

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr)
    {
        if (entry->d_type == DT_DIR)
        {
            pid_t child_pid = atoi(entry->d_name);
            if (child_pid <= 0) continue;

            std::string statPath = "/proc/" + std::to_string(child_pid) + "/stat";
            std::ifstream statFile(statPath);
            if (statFile.is_open())
            {
                std::string tmp;
                int ppid;
                statFile >> tmp >> tmp >> tmp >> ppid;
                if (ppid == pid)
                {
                    count++;
                }
            }
        }
    }
    closedir(dir);
    return count;
}


std::string getProcessMoreInfo(pid_t pid) {
    std::ostringstream out;

    // --- State, utime, stime, threads ---
    std::string state = "?";
    long utime = 0, stime = 0;
    int threads = 0;

    std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");
    if (statFile.is_open()) {
        std::string tmp, comm;
        char cstate;
        int ppid;
        for (int i = 0; i < 2; ++i) statFile >> tmp; // skip pid + comm
        statFile >> cstate;
        state = cstate;
        // skip to fields 14 and 15 (utime, stime)
        for (int i = 4; i <= 13; ++i) statFile >> tmp;
        statFile >> utime >> stime;
        // field 20 - number of threads
        for (int i = 16; i < 20; ++i) statFile >> tmp;
        statFile >> threads;
    }

    // --- Memory ---
    long mem_kb = 0;
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
    if (statusFile.is_open()) {
        std::string key;
        while (statusFile >> key) {
            if (key == "VmRSS:") {
                statusFile >> mem_kb;
                break;
            } else {
                std::string dummy;
                std::getline(statusFile, dummy);
            }
        }
    }

    // --- Total RAM ---
    long mem_total_kb = 1; // default to prevent division by zero
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        std::string line;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal:") != std::string::npos) {
                std::istringstream iss(line);
                std::string tmp;
                iss >> tmp >> mem_total_kb;
                break;
            }
        }
    }

    double mem_percent = (double)mem_kb / mem_total_kb * 100.0;

    // --- CPU usage approximation ---
    long ticks_per_sec = sysconf(_SC_CLK_TCK);
    double cpu_user_sec = (double)utime / ticks_per_sec;
    double cpu_sys_sec  = (double)stime / ticks_per_sec;

    // --- Command / Name ---
    std::string cmd;
    std::ifstream cmdFile("/proc/" + std::to_string(pid) + "/cmdline");
    if (cmdFile.is_open()) {
        std::getline(cmdFile, cmd, '\0'); // read until null terminator
    }

    // --- Map state character to string ---
    std::string stateStr;
    switch (state[0]) {
        case 'R': stateStr = "Running"; break;
        case 'S': stateStr = "Sleeping"; break;
        case 'D': stateStr = "Disk Sleep"; break;
        case 'Z': stateStr = "Zombie"; break;
        case 'T': stateStr = "Stopped"; break;
        case 't': stateStr = "Tracing Stop"; break;
        case 'X': stateStr = "Dead"; break;
        default:  stateStr = "Unknown"; break;
    }

    // --- Build output string ---
    out << "State: " << stateStr << "\n";
    out << "CPU: " << cpu_user_sec + cpu_sys_sec << " sec\n";
    out << "Memory: " << (mem_kb / 1024.0) << " MB (" << mem_percent << "%)\n";
    out << "Threads: " << threads << "\n";
    out << "Command: " << cmd << "\n";

    return out.str();
}
