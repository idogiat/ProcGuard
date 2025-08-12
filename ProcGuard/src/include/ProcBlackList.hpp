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

    void initSharedMemory();
    ProcBlackList();
    ~ProcBlackList();

public:
    static ProcBlackList& getInstance(void);

    /*
    * @brief: Add new process id to shared memory.
    * @retval: id index in memory else 0
    */
    int addPid(uint16_t pid);
    int removePid(uint16_t pid);
    bool isExists(uint16_t pid);
};