#pragma once 
#include <stdint.h>



enum class ProcStatus : uint8_t
{
    OK = 0,
    WAITING = 1,
    CHECKING = 2,
    WARNING = 3,
    NOT_EXISTS = 0xFF,
};


enum class ProcType : uint8_t
{
    CPU = 0,
    MEMORY = 1,
};
