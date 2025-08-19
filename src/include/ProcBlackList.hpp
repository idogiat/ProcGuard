#pragma once
#include <cstdint>
#include <string>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/unordered_map.hpp>
#include <functional>


using namespace boost::interprocess;

constexpr size_t MAX_PROCESSES = 256;

// hsm boost shared memory unordered_map
typedef managed_shared_memory::segment_manager segment_manager_t;
typedef allocator<std::pair<const uint16_t, char>, segment_manager_t> ShmemAllocator;
typedef boost::unordered_map<uint16_t, char, std::hash<uint16_t>, std::equal_to<uint16_t>, ShmemAllocator> PidHashMap;

class ProcBlackList
{
private:
    static constexpr size_t MAX_PROCESSES = 256;
    const std::string shm_name_ = "proc_guard_blacklist_shm";
    const std::string map_name_ = "proc_guard_blacklist_map";
    managed_shared_memory shm_;
    PidHashMap* pid_map_;

    /*     * @brief: Initialize shared memory and map.
     * @retval: 0 on success, -1 on failure
     * @note: This function should be called only once at the start of the program.
     */
    void initSharedMemoryAndMap();

    /*
     * @brief: Clean up shared memory and map.
     * @note: This function should be called at the end of the program.
     */
    void cleanupSharedMemoryAndMap();

    ProcBlackList(const ProcBlackList&) = delete;
    ProcBlackList& operator=(const ProcBlackList&) = delete;

protected:
    /*
     * @brief: Constructor to initialize shared memory and map.
     * @note: This function should be called only once at the start of the program.
     */
    void initSharedMemory();

    /*
     * @brief: Constructor to initialize shared memory and map.
     * @note: This function should be called only once at the start of the program.
     */
    ProcBlackList();
    ~ProcBlackList();

public:
    /* 
     * @brief: Get the singleton instance of ProcBlackList.
     * @return Reference to the singleton instance.
     */
    static ProcBlackList& getInstance(void);

    /*
     * @brief: Add new process id to shared memory.
     * @retval: id index in memory else 0
     */
    int addPid(uint16_t pid);

    /* 
     * @brief: Remove process id from shared memory.
     * @param pid The PID of the process to remove.
     * @retval: 0 on success, -1 on failure
     */
    int removePid(uint16_t pid);

    /*
     * @brief: Check if a process id exists in shared memory.
     * @param pid The PID of the process to check.
     * @retval: true if exists, false otherwise
     */
    bool isExists(uint16_t pid);
};