// Suspect Process Memory Handler

#pragma once
#include "ISPHandler.hpp"
#include "DStraceHandler.hpp"


class SPMemHandler : public ISPHandler
{
private:
    DStraceHandler *m_strace_handler;

public:
    SPMemHandler(int pid);
    ~SPMemHandler();
    int watch(int intervals = INTERVALS_DEFAULT, int time = TIME_DEFAULT) override;
    int analyze();
};
