#pragma once
#include <string>


/*
 * Utility functions to get process information from /proc filesystem
 */

/*
 * get process name by pid
*/
std::string getProcessName(pid_t pid);

/* 
 * get process owner by pid
 */
std::string getProcessOwner(pid_t pid);

/*
 * count number of child processes for given pid
 */
int countChildren(pid_t pid);

/*
 * get more detailed process info: CPU time, memory usage, context switches
 */
std::string getProcessMoreInfo(pid_t pid);