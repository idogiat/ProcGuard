#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <chrono>
#include <thread>
#include "shared_mem.hpp"

int main() {
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    void* ptr = mmap(nullptr, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    auto* counter = static_cast<int*>(ptr);

    sem_t* sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    for (int i = 0; i < 1000; ++i) {
        sem_wait(sem);
        std::cout << "Reader: counter = " << *counter << std::endl;
        sem_post(sem);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    munmap(ptr, SHM_SIZE);
    close(shm_fd);
    sem_close(sem);
    return 0;
}