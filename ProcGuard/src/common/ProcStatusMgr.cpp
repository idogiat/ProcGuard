#include "ProcStatusMgr.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdexcept>
#include <cstring>
#include <iostream>


ProcStatusMgr& ProcStatusMgr::getInstance(void)
{
    static ProcStatusMgr instance;
    return instance;
}

int ProcStatusMgr::addPid(pid_t pid)
{
    int index = 0;
    pthread_mutex_lock(&shm_ptr_->lock);
    for (size_t i = 0; i < shm_ptr_->p_count; ++i)
    {
        if (shm_ptr_->entries[i].pid == pid)
        {
            pthread_mutex_unlock(&shm_ptr_->lock);
            return index;
        }
    }

    if (shm_ptr_->p_count < MAX_PROCESSES)
    {
        index = shm_ptr_->p_count;
        shm_ptr_->entries[index].pid = pid;
        shm_ptr_->entries[index].status = ProcStatus::WAITING;
        shm_ptr_->entries[index].active = true;
        shm_ptr_->p_count++;
    }
    else
    {
        std::cerr << "Failed set status, memory is full" << std::endl;
    }

    pthread_mutex_unlock(&shm_ptr_->lock);
    return index;
}

void ProcStatusMgr::setStatus(pid_t pid, ProcStatus status)
{
    pthread_mutex_lock(&shm_ptr_->lock);

    for (size_t i = 0; i < shm_ptr_->p_count; ++i)
    {
        if (shm_ptr_->entries[i].active && shm_ptr_->entries[i].pid == pid)
        {
            shm_ptr_->entries[i].status = status;
            pthread_mutex_unlock(&shm_ptr_->lock);
            return;
        }
    }

    pthread_mutex_unlock(&shm_ptr_->lock);
    throw std::runtime_error("Failed set status, pid not exists");
}

ProcStatus ProcStatusMgr::getStatus(pid_t pid)
{
    ProcStatus status = ProcStatus::NOT_EXISTS;
    pthread_mutex_lock(&shm_ptr_->lock);
    for (size_t i = 0; i < shm_ptr_->p_count; ++i)
    {
        if (shm_ptr_->entries[i].pid == pid)
        {
            status = shm_ptr_->entries[i].status;
            break;
        }
    }
    
    pthread_mutex_unlock(&shm_ptr_->lock);
    return status;
}

void ProcStatusMgr::removeStatus(pid_t pid)
{
    pthread_mutex_lock(&shm_ptr_->lock);
    for (size_t i = 0; i < shm_ptr_->p_count; ++i)
    {
        if (shm_ptr_->entries[i].pid == pid)
        {
            shm_ptr_->entries[i].status = shm_ptr_->entries[shm_ptr_->p_count].status;
            shm_ptr_->entries[i].pid = shm_ptr_->entries[shm_ptr_->p_count].pid;
            shm_ptr_->entries[i].active = shm_ptr_->entries[shm_ptr_->p_count].active;
            shm_ptr_->entries[shm_ptr_->p_count].active = false;
            shm_ptr_->p_count--;
            break;
        }
    }
    
    pthread_mutex_unlock(&shm_ptr_->lock);
}

// Private functions
ProcStatusMgr::ProcStatusMgr()
{
    initSharedMemory();
    if (!created_)
    {
        initMutex();
        created_ = true;
        shm_ptr_->ref_count.store(1);
    }
    else
    {
        shm_ptr_->ref_count.fetch_add(1);
    }
}

ProcStatusMgr::~ProcStatusMgr()
{
    if (!shm_ptr_)
        return;

    pthread_mutex_lock(&shm_ptr_->lock);
    int count = --(shm_ptr_->ref_count);
    pthread_mutex_unlock(&shm_ptr_->lock);

    munmap(shm_ptr_, sizeof(ProcStatusShm));
    close(shm_fd_);

    if (count == 0) {
        shm_unlink(shm_name_.c_str());
    }
}

void ProcStatusMgr::initSharedMemory()
{
    shm_fd_ = shm_open(shm_name_.c_str(), O_RDWR, 0666);
    if (shm_fd_ == -1)
    {
        shm_fd_ = shm_open(shm_name_.c_str(), O_CREAT | O_RDWR, 0666);
        if (shm_fd_ == -1)
        {
            throw std::runtime_error("Failed to create shared memory");
        }
        if (ftruncate(shm_fd_, sizeof(ProcStatusShm)) == -1)
        {
            throw std::runtime_error("Failed to set shared memory size");
        }
        created_ = true;
    }

    void* ptr = mmap(nullptr, sizeof(ProcStatusShm), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);
    if (ptr == MAP_FAILED) {
        close(shm_fd_);
        throw std::runtime_error("Failed to mmap shared memory");
    }

    shm_ptr_ = static_cast<ProcStatusShm*>(ptr);

    if (created_) {
        std::memset(shm_ptr_, 0, sizeof(ProcStatusShm));
    }
}

void ProcStatusMgr::initMutex()
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shm_ptr_->lock, &attr);
    pthread_mutexattr_destroy(&attr);
}