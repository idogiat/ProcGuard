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
        case ProcStatus::WAITING: return "WAITING";
        case ProcStatus::CHECKING: return "CHECKING";
        case ProcStatus::SUSPICIOUS: return "SUSPICIOUS";
        case ProcStatus::ANALYZE_ERROR: return "ANALYZE_ERROR";
        case ProcStatus::NOT_EXISTS: return "NOT_EXISTS";
        default: return "UNKNOWN";
    }
}

enum class ProcType : uint8_t
{
    MEMORY = 0,
    CPU = 1,
    DEADLOCK = 2,
    RSS = 3,
    // NETWORK = 4,
    // FILES_IO = 5,
    // SYSCALL = 6,
    // SUB_PROCESSES = 6,
    OTHER = 0xFF,
};