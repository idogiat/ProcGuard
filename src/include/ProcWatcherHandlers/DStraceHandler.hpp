// Interface Data Strace Handler
#pragma once
#include "IDHandler.hpp"


class DStraceHandler : public IDHandler
{
public:
    static constexpr const char* COLLECT_FILE = "strace.log";
    static constexpr const char* ANALYZE_FILE = "strace.json";

public:
    DStraceHandler();
    ~DStraceHandler() override = default;
    
    int collect(const std::filesystem::path& folder_path, int pid, int time) override;
    int analyze(const std::filesystem::path& folder_path, int pid) override;
};