# Process Behavior Mapping – Information Collection Strategy

This document explains the purpose of the Process Monitoring page:
to present methods and tools for collecting information about a suspicious process, and to outline a strategy for building a behavioral map that reflects how the process operates over time.

## Goals
- Collect relevant runtime data about a process under investigation.
- Use multiple Linux tools to provide different perspectives (system calls, memory, CPU, I/O, etc.).
- Correlate the data into a coherent timeline that tells the “story” of the process.

## Tools and Their Roles
 - `/proc/\<pid>/status` – quick snapshot of memory usage (VmRSS, VmSize, VmSwap), threads, and state
 - `/proc/\<pid>/smaps / pmap` – detailed memory map: heap, stack, shared libs, and growth patterns.
 - `perf trace` – efficient tracing of syscalls, showing what the process does (I/O, sleep, memory alloc/free).
 - `perf stat / perf` record – performance counters: CPU cycles, context switches, cache misses, hotspots.
 - `strace` (optional) – verbose syscall log, useful for debugging but heavier than perf trace
 - `valgrind massif` (offline) – deep heap profiling, timeline of allocations (good for testing, heavy for prod)

## Relevant Information to Collect


## Strategy for Building a Process Map
