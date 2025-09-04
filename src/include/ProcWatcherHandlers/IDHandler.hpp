// Interface Data Handler
#pragma once

#include <string>
#include <filesystem>

class IDHandler
{
protected:
    int pid;
    std::filesystem::path file_collect;
    std::filesystem::path file_analyze;

public:
    explicit IDHandler(const std::filesystem::path& folder_path, int pid) : pid(pid) {};

    virtual ~IDHandler() = default;

    virtual int collect(int time) = 0;
    virtual int analyze() = 0;
};