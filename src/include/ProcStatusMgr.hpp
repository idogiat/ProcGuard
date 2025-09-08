#pragma once
#include "commonTypes.h"
#include <cstdint>
#include <string>
#include <pthread.h>
#include <atomic>
#include <vector>


struct PidEntry
{
    pid_t pid;
    ProcStatus status;
    ProcType detect_type;
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
    const std::string shm_name_ = "/proc_guard_status_mgr_shm";
    bool created_ = false;
    
    /* 
     * @brief: Initialize shared memory and mutex.
     * @retval: 0 on success, -1 on failure
     * @note: This function should be called only once at the start of the program.
    */
    void initSharedMemory();

    /*
     * @brief: Initialize mutex for shared memory.
     * @note: This function should be called only once at the start of the program.
     */
    void initMutex();

    /*
     * @brief: Clean up shared memory and mutex.
     * @note: This function should be called at the end of the program.
     */
    void cleanupSharedMemory();

    /* 
     * @brief: Find the index of a process in shared memory.
     * @param pid The PID of the process to find.
     * @return The index of the process in shared memory, or -1 if not found.
     */
    int findPidIndex(pid_t pid);

    ProcStatusMgr();
    ~ProcStatusMgr();

public:
    static ProcStatusMgr& getInstance(void);

    /*
     * @brief: Add new process id to shared memory.
     * @param pid The PID of the process to add.
     * @param detect_type The type of detection for the process.
     * @retval: id index in memory else 0
     * 
     */
    int addPid(pid_t pid, ProcType detect_type);

    /*
     * @brief: Remove process id from shared memory.
     * @retval: 0 on success, -1 if pid not found
     */
    int removePid(pid_t pid);
    
    /*
     * @brief: Set the status of a process in shared memory.
     * @param pid The PID of the process.
     * @param status The status to set for the process.
     * @note: This function should be called only when the process is being monitored.
     */
    void setStatus(pid_t pid, ProcStatus status);

    /*
     * @brief: Get the detection type of a process in shared memory.
     * @param pid The PID of the process.
     * @return The detection type of the process, or ProcType::OTHER if the process is not found.
     */
    ProcType getDetectedType(pid_t pid);
    
    /* 
     * @brief: Get the status of a process in shared memory.
     * @param pid The PID of the process.
     * @return The status of the process, or ProcStatus::NOT_EXISTS if the process is not found.
     */
    ProcStatus getStatus(pid_t pid);
    
    /*
     * @brief: Get all process IDs in shared memory.
     * @return A vector of PidEntry containing all process IDs and their statuses.
     * @note: This function should be called to get the list of all monitored processes.
     */
    std::vector<PidEntry> getAllPIDs(void);

    /* 
     * @brief: Check if a process exists in shared memory.
     * @param pid The PID of the process.
     * @return true if the process exists, false otherwise.
     */
    bool isExists(pid_t pid);
};