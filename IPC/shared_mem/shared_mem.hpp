#pragma once

#include <cstddef>

constexpr const char* SHM_NAME = "/my_shared_memory";
constexpr const char* SEM_NAME = "/my_shared_semaphore";
constexpr std::size_t SHM_SIZE = sizeof(int);