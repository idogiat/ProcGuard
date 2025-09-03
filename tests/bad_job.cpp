#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>
#include <sys/syscall.h>

int main() {
    const int BLOCKS = 200;
    const size_t BLOCK_SIZE = 1024*1024;
    std::vector<std::vector<char>> memory_blocks;
    for (int i = 0; i < BLOCKS; ++i) {
        memory_blocks.push_back(std::vector<char>(BLOCK_SIZE, 'x'));
    }
    std::cout << "Allocated " << BLOCKS << " MB of memory.\n";

    auto cpu_intensive = []() {
        volatile double x = 0;
        while (true) {
            for (int i = 0; i < 1000000; ++i) {
                x += i * 0.0001;
            }
            syscall(SYS_gettid);
            write(STDOUT_FILENO, ".", 1);
        }
    };

    std::thread t1(cpu_intensive);
    std::thread t2(cpu_intensive);

    t1.join();
    t2.join();

    return 0;
}
