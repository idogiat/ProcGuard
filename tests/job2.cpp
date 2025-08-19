#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <csignal>   // for signal handling
#include <atomic>
#include <unistd.h>  // for getpid()

std::atomic<int> sigusr1_count{0};

void signal_handler(int signum) {
    if (signum == SIGUSR1) {
        ++sigusr1_count;
        std::cout << "\nCaught SIGUSR1 (" << sigusr1_count.load() << " times)." << std::endl;
    }
}

void do_heavy_job(size_t target_bytes) {
    {
        std::vector<char> mem(target_bytes, 42);

        std::cout << "Allocated " << target_bytes / (1024 * 1024) << " MB of memory." << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        while ((std::chrono::high_resolution_clock::now() - start).count() < 25'000'000) {
            for (size_t i = 0; i < mem.size(); i += 4096) {
                mem[i]++;
            }
        }

        std::cout << "Finished heavy job, freeing memory now." << std::endl;
    }

    std::cout << "Memory freed after leaving scope." << std::endl;
}

int main() {
    std::signal(SIGUSR1, signal_handler);

    size_t mb = 1024 * 1024;
    size_t chunk_size = 512 * mb;
    int cycles = 10;

    pid_t pid = getpid();

    while (true) {
        for (int i = 0; i < cycles; ++i) {
            std::cout << "Cycle " << i + 1 << "/" << cycles << std::endl;
            do_heavy_job(chunk_size);

            // Sleep a bit to let system breathe
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // Send SIGINT signal to self every 3 cycles as example
            if ((i + 1) % 3 == 0) {
                std::cout << "Sending SIGUSR1 signal to self." << std::endl;
                // Option 1: Using raise()
                raise(SIGUSR1);
                // OR Option 2: Using kill()
                // kill(pid, SIGINT);
            }
        }
    }

    return 0;
}
