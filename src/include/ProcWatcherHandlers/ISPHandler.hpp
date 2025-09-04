// Interface Suspect Process Handler

#pragma once

#include <string>
#include <filesystem>


class ISPHandler
{
private:
    int pid;

protected:
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

    virtual ~ISPHandler() {};
    virtual void watch() = 0;
    virtual void analyze() = 0;
};
