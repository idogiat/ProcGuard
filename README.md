# ProcGuard

`ProcGuard` is a real-time process monitoring system for Linux, combining C++ and Python.
The system tracks processes, performs strace, stores data in Shared Memory, and applies statistical/ML analysis to detect suspicious processes.

## Features

- Real-time process monitoring using strace.
- Analysis of system calls and signals.
- Shared Memory for storing process status.
- Python scripts leveraging ML (RandomForestClassifier) to identify suspicious processes.
- Support for running processes with root privileges (sudo) for strace.
- Structured logging for each monitored process.
- Extensible with a Qt GUI to visualize process statuses.

## Architecture
```
[C++ Core] <-> [Shared Memory / MsgQueue] <-> [Python Scripts]
     |
     +--> strace -> log -> strace_parser.py -> JSON -> proc_analyzer.py -> ML
```

- C++ Core: manages processes, shared memory, MsgQueue, fork, and IPC.
- Python Scripts:
    - strace_parser.py – parses strace output into JSON.
    - proc_analyzer.py – analyzes JSON data with ML and returns process status.
    ML: RandomForestClassifier trained on strace data.

## System Requirements

- Linux (Ubuntu recommended)
- C++17
- Python 3.8+
- strace
- Qt5/Qt6 (for GUI)

## Installation

1. Create a Python virtual environment:
```
python3 -m venv VENV
source VENV/bin/activate
pip install pandas scikit-learn
```

2. Compile C++ Core:
```
cmake -B build -S .
make -C build
```

3. Run ProcGuard:
```
source VENV/bin/activate
sudo -E python3 run.py --clean
```

Note: strace requires root privileges. Running ProcGuard with sudo ensures proper tracing of monitored processes.

## Usage

ProcGuard continuously listens for messages about processes to monitor.
Each process goes through the following steps:

1. strace traces the target process.
2. strace_parser.py parses logs into JSON.
3. proc_analyzer.py analyzes the data and applies ML for classification.
4. Process status is updated in Shared Memory.

## Logging

Logs are stored in the logs/ folder, one file per process:
```
logs/proc_<PID>.log
```

Python scripts also generate intermediate JSON files in `/tmp/procguard_json/`.

## GUI (Future / Optional)

- Qt-based window to display active monitored processes.
- Show real-time status and analytics per process.
- Read directly from shared memory.