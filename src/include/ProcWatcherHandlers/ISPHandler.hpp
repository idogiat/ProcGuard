// Interface Suspect Process Handler

#pragma once

#include <string>
#include <filesystem>

#define INTERVALS_DEFAULT 3
#define TIME_DEFAULT 5

class ISPHandler
{
protected:
    int pid;
    std::filesystem::path pid_folder;

public:
    ISPHandler(int pid) : pid(pid)
    {
        pid_folder = std::filesystem::path("/tmp/procguard_log/pid_" + std::to_string(pid) + "/");
        if (!std::filesystem::exists(pid_folder))
        {
            std::filesystem::create_directories(pid_folder);
        }
    }

    virtual ~ISPHandler() = default;

    /*
     * Watch the suspect process for a certain number of intervals,
     * each interval lasting a certain amount of time.
     * Returns 0 on success, non-zero on failure.
    */
    virtual int watch(int intervals = INTERVALS_DEFAULT, int time = TIME_DEFAULT) = 0;

    /*
     * Analyze the collected data.
     * Returns 0 on success, non-zero on failure.
    */
    virtual int analyze() = 0;
};
