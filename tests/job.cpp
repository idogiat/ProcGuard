#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

void do_heavy_job(size_t target_bytes) {
    {
        // Allocate memory in a vector
        std::vector<char> mem(target_bytes, 42);

        std::cout << "Allocated " << target_bytes / (1024 * 1024) << " MB of memory." << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        // Perform some heavy work by iterating over the memory every 4096 bytes
        while ((std::chrono::high_resolution_clock::now() - start).count() < 25'000'000) {
            for (size_t i = 0; i < mem.size(); i += 4096) {
                mem[i]++;
            }
        }

        std::cout << "Finished heavy job, freeing memory now." << std::endl;
    } // 'mem' goes out of scope here and memory is automatically freed

    std::cout << "Memory freed after leaving scope." << std::endl;
}

int main() {
    size_t mb = 1024 * 1024;
    size_t chunk_size = 512 * mb;  // 512MB per allocation
    int cycles = 10;

    while (true) {
        for (int i = 0; i < cycles; ++i) {
            std::cout << "Cycle " << i + 1 << "/" << cycles << std::endl;
            do_heavy_job(chunk_size);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Small pause to let the system breathe
        }
    }

    return 0;
}
