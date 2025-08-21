#include "ProcBlackList.hpp"

ProcBlackList::ProcBlackList() : shm_(open_or_create, shm_name_.c_str(), ProcBlackList::MAX_PROCESSES * 16)
{
    initSharedMemory();
}

ProcBlackList::~ProcBlackList()
{
    // Optionally, remove shared memory on destruction
    // managed_shared_memory::remove(shm_name_.c_str());
}

void ProcBlackList::initSharedMemory()
{
    pid_map_ = shm_.find_or_construct<PidHashMap>(map_name_.c_str())
        (ProcBlackList::MAX_PROCESSES, std::hash<uint16_t>(), std::equal_to<uint16_t>(), shm_.get_segment_manager());
}

ProcBlackList& ProcBlackList::getInstance()
{
    static ProcBlackList instance;
    return instance;
}

int ProcBlackList::addPid(uint16_t pid)
{
    auto result = pid_map_->insert(std::make_pair(pid, 1));
    return result.second ? 1 : 0; // 1 if inserted, 0 if already exists
}

int ProcBlackList::removePid(uint16_t pid)
{
    return pid_map_->erase(pid); // returns number of elements removed (0 or 1)
}

bool ProcBlackList::isExists(uint16_t pid)
{
    return pid_map_->find(pid) != pid_map_->end() ? true : false;
}