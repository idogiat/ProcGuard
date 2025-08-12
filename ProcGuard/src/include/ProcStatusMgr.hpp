#pragma once
#include "common.h"
#include <cstdint>
#include <string>
#include <pthread.h>
#include <atomic>


struct PidEntry
{
    pid_t pid;
    ProcStatus status;
    bool active;
};


class ProcStatusMgr
{
    private:
    static constexpr size_t MAX_PROCESSES = 256;
    struct ProcStatusShm
    {
        pthread_mutex_t lock;
        std::atomic<int> ref_count;
        uint8_t p_count;
        PidEntry entries[MAX_PROCESSES];
    };
    
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

    /*
    * @brief: Add new process id to shared memory.
    * @retval: id index in memory else 0
    */
    int addPid(pid_t pid);

    void setStatus(pid_t pid, ProcStatus status);
    void removeStatus(pid_t pid);
    ProcStatus getStatus(pid_t pid);
};