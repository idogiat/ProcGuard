// Interface Data Strace Handler
#include "DStraceHandler.hpp"
#include "commonPathes.h"
#include <iostream>
#include <filesystem>

DStraceHandler::DStraceHandler() : IDHandler(DStraceHandler::COLLECT_FILE, DStraceHandler::ANALYZE_FILE) {}

int DStraceHandler::collect(const std::filesystem::path& folder_path, int pid, int time)
{
    std::string file_collect = (folder_path / m_file_collect).string();
    std::string file_analyze = (folder_path / m_file_analyze).string();

    std::string strace_cmd = "sudo timeout " + std::to_string(time) + "s strace -e trace=all -f -s 0 -yy -ttt -o " +
                             file_collect + " -p " + std::to_string(pid);

    std::string parser_cmd = std::string(PYTHON_PATH) + " " + std::string(PARSER_SCRIPT) + 
                                " -s " + file_collect +
                                " -j " + file_analyze;
        
    int ret = system(strace_cmd.c_str());
    if (ret == 0)
    {
        ret = system(parser_cmd.c_str());
    }

    return WEXITSTATUS(ret);
}

int DStraceHandler::analyze(const std::filesystem::path& folder_path, int pid)
{
    std::string file_analyze = (folder_path / m_file_analyze).string();

    std::string analyzer_cmd = std::string(PYTHON_PATH) + " " + std::string(ANALYZER_SCRIPT) +
                                " -j " + file_analyze +
                                " -d " +  std::string(DATA_FILE) +
                                " -p " + std::to_string(pid);
    
    int ret = system(analyzer_cmd.c_str());

    return WEXITSTATUS(ret);
}