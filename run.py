import argparse
import os
import signal
import subprocess

processes = {}

LOG_FOLDER = "logs"
VENV_PATH = "./VENV/bin/python3"


def start_process(name, command, shell=False):
    """Start a process and redirect its output to a log file."""
    stdout = open(f"{LOG_FOLDER}/{name}.log", "w")
    stderr = subprocess.STDOUT

    proc = subprocess.Popen(command, stdout=stdout, stderr=stderr, preexec_fn=os.setsid, shell=shell)
    processes[name] = proc
    print(f"Process '{name}' started with PID {proc.pid}.")


def stop_process(name):
    """Stop a process by name."""
    proc = processes.get(name)
    if not proc:
        return
    try:
        os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        proc.wait()
        print(f"Process '{name}' stopped.")
    except Exception as e:
        print(f"Error stopping process '{name}': {e}")
    finally:
        del processes[name]

def stop_all_processes(signum=None, frame=None):
    print("\nCTRL+C detected. Stopping all processes...")
    for name in list(processes.keys()):
        stop_process(name)
    exit(0)

signal.signal(signal.SIGINT, stop_all_processes)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Start and manage processes for ProcWatcher.")
    parser.add_argument("--clean", action="store_true", help="Clean up log files, and memory before starting processes.")
    args = parser.parse_args()
    
    if args.clean:
        print("Cleaning up log files and memory...")
        os.system("rm ps_snapshot.db")
        os.system("rm -rf logs/*")
        os.system("rm -r /tmp/procguard_*")
        os.system("rm -r /dev/shm/proc_guard_*")


    os.makedirs(LOG_FOLDER, exist_ok=True)
    start_process("proc_scanner", ["source ./VENV/bin/activate & python3 proc_scanner/proc_scanner.py"], shell=True)
    start_process("ProcWatcher", ["source ./VENV/bin/activate & ./build/ProcWatcher"], shell=True)
    start_process("ProcMonitor", ["./build/ProcMonitor"])
    start_process("ProcMonitor", ["./build/ProcGui"])

    print("Processes running. Press CTRL+C to stop all.")
    signal.pause()
