#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex mutexA;
std::mutex mutexB;

void thread1() {
    std::cout << "Thread 1: Trying to lock mutexA\n";
    std::lock_guard<std::mutex> lockA(mutexA); 
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Thread 1: Trying to lock mutexB\n";
    std::lock_guard<std::mutex> lockB(mutexB); 
    std::cout << "Thread 1: Locked both mutexes\n";
}

void thread2() {
    std::cout << "Thread 2: Trying to lock mutexB\n";
    std::lock_guard<std::mutex> lockB(mutexB);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Thread 2: Trying to lock mutexA\n";
    std::lock_guard<std::mutex> lockA(mutexA);
    std::cout << "Thread 2: Locked both mutexes\n";
}

int main() {
    std::thread t1(thread1);
    std::thread t2(thread2);

    t1.join();
    t2.join();

    std::cout << "Finished\n";
    return 0;
}
