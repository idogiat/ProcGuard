#pragma once

#include <cstdint>
#include <string>
#include <pthread.h>
#include <atomic>


constexpr size_t MAX_PROCESSES = 256;


enum class ProcStatus : uint8_t
{
    OK = 0,
    WAITING = 1,
    CHECKING = 2,
    WARNING = 3,
};

struct PidEntry
{
    pid_t pid;
    ProcStatus status;
    bool active;
};

struct ProcStatusShm
{
    pthread_mutex_t lock;
    std::atomic<int> ref_count;
    PidEntry entries[MAX_PROCESSES];
};


class ProcStatusMgr
{
private:
    int shm_fd_ = -1;
    ProcStatusShm* shm_ptr_ = nullptr;
    const std::string shm_name_ = "/proc_guard_shm";
    bool created_ = false;

    void initSharedMemory();
    void initMutex();
    ProcStatusMgr();
    ~ProcStatusMgr();
public:

    static ProcStatusMgr& getInstance(void);
    void setStatus(pid_t pid, ProcStatus status);
    void removeStatus(pid_t pid);
    ProcStatus getStatus(pid_t pid);
};