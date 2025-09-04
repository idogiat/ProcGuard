// Suspect Process Memory Handler

#pragma once
#include "ISPHandler.hpp"


class SPMemHandler : public ISPHandler
{
public:
    SPMemHandler(int pid);
    ~SPMemHandler() {};
    void watch();
    void analyze();
};
