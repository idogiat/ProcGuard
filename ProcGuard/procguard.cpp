#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

// Function to read and print basic info about a process by its PID
void read_process_info(pid_t pid) {
    char path[256];
    char buffer[1024];

    // Open /proc/[pid]/stat to read process info
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE* stat_file = fopen(path, "r");
    if (!stat_file) return;

    if (fgets(buffer, sizeof(buffer), stat_file)) {
        // Parse the stat file:
        // Format includes pid, comm (process name), state, ppid, etc.
        char comm[256];
        char state;
        int ppid;
        sscanf(buffer, "%*d (%[^)]) %c %d", comm, &state, &ppid);
        printf("PID: %d, Name: %s, State: %c, PPID: %d\n", pid, comm, state, ppid);
    }

    fclose(stat_file);

    // Open /proc/[pid]/status to read memory usage info
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE* status_file = fopen(path, "r");
    if (!status_file) return;

    // Look for VmRSS line (resident set size - actual memory usage)
    while (fgets(buffer, sizeof(buffer), status_file)) {
        if (strncmp(buffer, "VmRSS:", 6) == 0) {
            printf("%s", buffer);
            break;
        }
    }

    fclose(status_file);
}

int main() {
    DIR* proc_dir = opendir("/proc");
    struct dirent* entry;

    if (!proc_dir) {
        perror("opendir");
        return 1;
    }

    // Iterate over entries in /proc directory
    while ((entry = readdir(proc_dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Check if directory name is a number (PID)
            pid_t pid = atoi(entry->d_name);
            if (pid > 0) {
                read_process_info(pid);
            }
        }
    }

    closedir(proc_dir);
    return 0;
}
