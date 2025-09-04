// Suspect Process Deadlock Handler

#pragma once

#include "ISPHandler.hpp"


class SPDeadLockHandler : public ISPHandler
{

public:
    SPDeadLockHandler(int pid);
    ~SPDeadLockHandler() {};
    void watch();
    void analyze();
};
