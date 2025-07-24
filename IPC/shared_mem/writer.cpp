#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <csignal>
#include <chrono>
#include <thread>
#include "shared_mem.hpp"

int main()
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(shm_fd, SHM_SIZE) == -1)
    {
        perror("ftruncate");
        return 1;
    }

    void* ptr = mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    auto* counter = static_cast<int*>(ptr);
    *counter = 0;

    sem_t* sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    for (int i = 0; i < 10; ++i) {
        sem_wait(sem);
        ++(*counter);
        std::cout << "Writer: counter = " << *counter << std::endl;
        sem_post(sem);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    sem_close(sem);
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);
    return 0;
}