// Interface Data Strace Handler
#pragma once
#include "IDHandler.hpp"

class DStraceHandler : public IDHandler
{
public:
    DStraceHandler(const std::filesystem::path& folder_path, int pid);
    ~DStraceHandler() override = default;

    int collect(int time) override;
    int analyze(void) override;
};