// Interface Data Strace Handler
#pragma once
#include "DStraceHandler.hpp"
#include "commonPathes.h"
#include <iostream>

DStraceHandler::DStraceHandler(const std::filesystem::path& folder_path, int pid) : IDHandler(folder_path, pid)
{
    file_collect = folder_path / "strace.log";
    file_analyze = folder_path / "strace.json";
}

int DStraceHandler::collect(int time)
{
    int ret;
    std::string strace_cmd = "sudo timeout " + std::to_string(time) + "s strace -e trace=all -f -s 0 -yy -ttt -o " +
                             file_collect + " -p " + std::to_string(pid);

    std::string parser_cmd = std::string(PYTHON_PATH) + " " + abs_parser_script_path + 
                                " -s " + file_collect +
                                " -j " + file_analyze;
        
    ret = system(strace_cmd.c_str());
    if (ret == 0)
    {
        ret = system(parser_cmd.c_str());
    }

    return WEXITSTATUS(ret);
}

int DStraceHandler::analyze(void)
{
    std::string analyzer_cmd = std::string(PYTHON_PATH) + " " + std::string(ANALYZER_SCRIPT) +
                                " -j " + file_analyze +
                                " -d " +  std::string(DATA_FILE) +
                                " -p " + std::to_string(pid);
    
    int ret = system(analyzer_cmd.c_str());
    return WEXITSTATUS(ret);
}