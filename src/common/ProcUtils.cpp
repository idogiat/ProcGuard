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


std::string getProcessMoreInfo(pid_t pid)
{
    std::ostringstream out;

    double cpu_user_sec = 0, cpu_sys_sec = 0;
    long memory_kb = 0;
    long voluntary_ctxt = 0, nonvoluntary_ctxt = 0;

    // --- CPU ---
    {
        std::string path = "/proc/" + std::to_string(pid) + "/stat";
        std::ifstream file(path);
        if (file.is_open())
        {
            std::string tmp;
            long utime, stime;
            for (int i = 1; i <= 13; i++) file >> tmp; // skip to 14th field
            file >> utime >> stime;
            long ticks_per_sec = sysconf(_SC_CLK_TCK);
            cpu_user_sec = (double)utime / ticks_per_sec;
            cpu_sys_sec  = (double)stime / ticks_per_sec;
        }
    }

    // --- Memory & Context Switches ---
    {
        std::string path = "/proc/" + std::to_string(pid) + "/status";
        std::ifstream file(path);
        std::string key;
        while (file >> key)
        {
            if (key == "VmRSS:")
            {
                file >> memory_kb;
            }
            else if (key == "voluntary_ctxt_switches:")
            {
                file >> voluntary_ctxt;
            }
            else if (key == "nonvoluntary_ctxt_switches:")
            {
                file >> nonvoluntary_ctxt;
            }
            else
            {
                std::string dummy;
                std::getline(file, dummy); // skip the rest of the line
            }
        }
    }

    out << "CPU User (" << cpu_user_sec << " sec), Sys (" << cpu_sys_sec << " sec)\n";
    out << "Mem Usage: " << memory_kb << " KB\n";
    out << "context Switch: " << voluntary_ctxt << " voluntary, " << nonvoluntary_ctxt << " non-voluntary";

    return out.str();
}