#include "DBManager.hpp"
#include <thread>
#include <chrono>
#include <iostream>

int main()
{
    DBManager *db = new DBManager(DB_PATH);

    while (true)
    {
        auto result1 = db->getMaxCPU(DB_NAME, 10);
        for (auto r : result1)
        {
            std::cout << "ID: " << std::get<0>(r) << " CPU: " << std::get<1>(r) << std::endl;
        }
        auto result2 = db->getMaxMEM(DB_NAME, 10);
        for (auto r : result2)
        {
            std::cout << "ID: " << std::get<0>(r) << " MEM: " << std::get<1>(r) << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));

    }
    
    return 0;
}