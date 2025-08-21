#pragma once 
#include <stdint.h>


enum class ProcStatus : uint8_t
{
    OK = 0,
    WAITING = 1,
    CHECKING = 2,
    SUSPICIOUS = 3,
    ANALYZE_ERROR = 4,
    NOT_EXISTS = 0xFF,
};

inline const char* ProcStatusToString(ProcStatus status) {
    switch (status) {
        case ProcStatus::OK: return "OK";
        case ProcStatus::CHECKING: return "CHECKING";
        case ProcStatus::SUSPICIOUS: return "SUSPICIOUS";
        case ProcStatus::ANALYZE_ERROR: return "ANALYZE_ERROR";
        default: return "UNKNOWN";
    }
}

enum class ProcType : uint8_t
{
    CPU = 0,
    MEMORY = 1,
};
