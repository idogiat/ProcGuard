// Interface Data Handler
#pragma once

#include <string>
#include <filesystem>

class IDHandler
{
protected:
    std::string m_file_collect;
    std::string m_file_analyze;

public:
    explicit IDHandler(std::string file_collect, std::string file_analyze)
    {
        m_file_collect = file_collect;
        m_file_analyze = file_analyze;
    }

    virtual ~IDHandler() = default;

    virtual int collect(const std::filesystem::path& folder_path, int pid, int time) = 0;
    virtual int analyze(const std::filesystem::path& folder_path, int pid) = 0;
};