cmake_minimum_required(VERSION 3.16)
project(ProcGuard)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt5
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src/include
                    ${CMAKE_SOURCE_DIR}/src/include/ProcWatcherHandlers)

# Common sources
set(COMMON_SRCS
    src/common/ProcStatusMgr.cpp
    src/common/DBMgr.cpp
    src/common/MsgQueue.cpp
    src/common/ProcBlackList.cpp
    src/common/ProcUtils.cpp
    )

set(PROC_WATCHER_SRCS
    src/common/ProcWatcherHandlers/DStraceHandler.cpp
    src/common/ProcWatcherHandlers/SPMemHandler.cpp
    )
    
# -------------------
# GUI target: ProcGui
# -------------------
add_executable(ProcGui
    src/targets/ProcGui.cpp
    src/common/ProcGuiHandler.cpp
    src/include/ProcGuiHandler.hpp
    ${COMMON_SRCS}
)

target_link_libraries(ProcGui Qt5::Widgets pthread rt sqlite3 stdc++fs)

# -----------------------
# CLI target: ProcMonitor
# -----------------------
add_executable(ProcMonitor
    src/targets/ProcMonitor.cpp
    ${COMMON_SRCS}
)

target_link_libraries(ProcMonitor pthread rt sqlite3 stdc++fs)

# -----------------------
# CLI target: ProcWatcher
# -----------------------
add_executable(ProcWatcher
    src/targets/ProcWatcher.cpp
    ${COMMON_SRCS}
    ${PROC_WATCHER_SRCS}
)

target_link_libraries(ProcWatcher pthread rt sqlite3 stdc++fs)


# -----------------------
# CLI target: ProcCleaner
# -----------------------
add_executable(ProcCleaner
    src/targets/ProcCleaner.cpp
    ${COMMON_SRCS}
)

target_link_libraries(ProcCleaner pthread rt sqlite3 stdc++fs)