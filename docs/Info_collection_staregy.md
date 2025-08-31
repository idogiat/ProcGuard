# Process Behavior Mapping – Information Collection Strategy

This document explains the purpose of the Process Monitoring page:
to present methods and tools for collecting information about a suspicious process, and to outline a strategy for building a behavioral map that reflects how the process operates over time.

## Goals
- Collect relevant runtime data about a process under investigation.
- Use multiple Linux tools to provide different perspectives (system calls, memory, CPU, I/O, etc.).
- Correlate the data into a coherent timeline that tells the “story” of the process.



## Process Behavior Analysis Workflow

The process monitoring and analysis strategy is divided into four main stages:

1. **Initial Flat Checks**  
   - Quickly assess the process status, ownership, PID, and basic resource usage.  
   - Determine if the process is alive, sleeping, or in a potentially stuck state.  
   - Tools: `ps`, `top`.

2. **Information Gathering & Basic Decision**  
   - Collect detailed info: subprocess count, memory footprint, CPU usage, context switches.  
   - Decide if deeper monitoring is necessary.  
   - Tools: `pmap`, simple `strace`, `/proc` readings.

3. **Deep Information Collection**  
   - Continuous monitoring to build a comprehensive process map.  
   - Collect syscall traces (`strace`), CPU/memory snapshots (`perf`, `pmap`), thread interactions, and resource waits.  
   - Data collected over time for ML analysis.

4. **Decision & Action**  
   - Analyze data (manually or with ML).  
   - Decide if process is normal, stuck, deadlocked, or suspicious.  
   - Take appropriate action (terminate, investigate further, allow continuation).

> This structured approach ensures a balance between lightweight monitoring and deep data collection.

---

### Visual Diagram

```
+-----------------------+
| 1. Initial Checks     |
|    (ps, top)          |
+-----------+-----------+
            |
            v
+---------------------------+
| 2. Info Gathering &       |
|    Basic Decision         |
|    (pmap, strace, /proc)  |
+-----------+---------------+
            |
            v
+-------------------------------+
| 3. Deep Info Collection       |
|    (strace, perf, pmap, ML)   |
+-----------+-------------------+
            |
            v
+-------------------------------+
| 4. Decision & Action          |
|    (Terminate / Investigate / |
|     Continue)                 |
+-------------------------------+

````



## Process Behavior Data Collection

This project focuses on **collecting informative data** to build a behavior map of a process. The goal is to monitor and analyze suspicious or critical processes for understanding their runtime behavior.

### Tools and Data Sources

1. **Syscalls / Active Behavior**  
   - Tools: `strace`, `perf trace`  
   - Data: system calls, arguments, return values, latency, success/failure  
   - Purpose: capture the process’s active operations (I/O, computation, waiting)

2. **Memory Usage**  
   - Tools: `/proc/PID/status`, `pmap`, `smem`  
   - Data: RSS, VMS, mapped files, page faults, swap usage  
   - Purpose: detect memory usage patterns, leaks, or unusual allocation

3. **CPU / Scheduler Info**  
   - Tools: `/proc/PID/stat`, `top`, `pidstat`, `perf stat`  
   - Data: CPU %, context switches, threads count, user/kernel time  
   - Purpose: understand resource utilization and process activity profile

4. **File / I/O / Network**  
   - Tools: `lsof`, `iotop`, `netstat`, `ss`  
   - Data: open files, sockets, I/O rate  
   - Purpose: monitor I/O behavior and network activity

5. **Kernel Waits / Locks**  
   - Tools: `/proc/PID/wchan`, `pstack`, `gdb`  
   - Data: threads’ wait locations, futex/mutex waits  
   - Purpose: identify deadlocks or blocking behavior

6. **Advanced / Optional**  
   - Tools: `eBPF / bpftrace`, `perf record`  
   - Data: detailed kernel events, CPU sampling, cache misses  
   - Purpose: fine-grained monitoring with low overhead

### Data Collection Strategy

- Sample process data periodically to create snapshots.  
- Extract features like CPU%, memory usage, syscall counts, wait types, and thread counts.  
- Use sequence or statistical models to analyze patterns (clustering, anomaly detection, or sequence modeling).  
- Build a **behavior map** for the process over time, enabling analysis of normal vs. abnormal behavior.

## Strategy for Building a Process Map

This strategy focuses on collecting detailed runtime information to understand a process's behavior, detect anomalies, or analyze potential deadlocks.

### 1. Identify Process and Scope
- Target a specific PID or a set of related processes.  
- Determine which aspects are critical: CPU, memory, syscalls, I/O, network, locks.

### 2. Collect Syscall & Execution Data
- Tools: `strace`, `perf trace`  
- Capture:
  - System calls (type, arguments, return value)
  - Timing of calls (latency)
  - Thread IDs performing the calls
- Strategy: sample periodically or trigger on specific events (e.g., blocking/futex waits)

### 3. Collect Memory Usage
- Tools: `/proc/PID/status`, `pmap`, `smem`  
- Capture:
  - RSS, VMS, stack size, heap, mapped files
  - Swap usage and page faults
- Strategy: sample at intervals; look for unusual growth or fragmentation

### 4. Collect CPU and Scheduler Info
- Tools: `/proc/PID/stat`, `pidstat`, `perf stat`  
- Capture:
  - CPU utilization (% user/system)
  - Context switches (voluntary/involuntary)
  - Number of threads, runtime per thread

### 5. Track Locks and Waiting
- Tools: `/proc/PID/wchan`, `gdb`, `pstack`  
- Capture:
  - Threads blocked on futexes or I/O
  - Function or syscall where the wait occurs
- Strategy: identify patterns of contention or potential deadlocks

### 6. Monitor I/O and Network Activity
- Tools: `lsof`, `iotop`, `netstat`, `ss`  
- Capture:
  - Open files, sockets
  - I/O rate per fd
  - Network connections (local/remote, sent/received bytes)

### 7. Organize and Store Data
- Structure collected data in JSON/CSV or a database.  
- Normalize units (MB, ms, counts).  
- Include timestamps for temporal correlation.

### 8. Build the Behavior Map
- Combine all collected metrics per time slice to generate a timeline.  
- Visualize:
  - Syscall frequency heatmaps
  - Memory and CPU usage over time
  - Thread activity and waits
- Use this map to identify abnormal behavior, performance bottlenecks, or deadlocks.

### 9. Optional Advanced Insights
- Tools: `eBPF`, `bpftrace` for kernel-level events  
- Capture cache misses, page faults, scheduling latency for deeper analysis.

> Goal: create a **comprehensive and dynamic map** of a process that reflects its operational behavior, resource usage, and interactions over time.


## Example

```json
{
  "pid": 7499,
  "process_name": "deadlock_job",
  "start_time": "2025-08-31T15:00:00Z",
  "monitoring_interval_ms": 1000,
  "snapshots": [
    {
      "timestamp": "2025-08-31T15:00:01Z",
      "cpu": {
        "total_usage_percent": 4.5,
        "per_thread": [
          {"tid": 7499, "usage_percent": 3.0},
          {"tid": 7502, "usage_percent": 1.5}
        ],
        "context_switches": {
          "voluntary": 12,
          "involuntary": 5
        }
      },
      "memory": {
        "rss_mb": 82.5,
        "vms_mb": 900.0,
        "swap_mb": 0.0,
        "page_faults": 23
      },
      "syscalls": [
        {"name": "write", "count": 5, "avg_latency_ms": 0.05},
        {"name": "futex", "count": 2, "avg_latency_ms": 15.0}
      ],
      "threads": [
        {
          "tid": 7499,
          "state": "running",
          "waiting_on": null
        },
        {
          "tid": 7502,
          "state": "sleeping",
          "waiting_on": "futex"
        }
      ],
      "io": {
        "files_opened": ["/dev/pts/3", "/tmp/log.txt"],
        "read_bytes": 1024,
        "written_bytes": 2048,
        "network_connections": [
          {"fd": 5, "remote": "192.168.1.10:443", "sent_bytes": 512, "recv_bytes": 1024}
        ]
      }
    }
  ]
}
```