import os
import re
import json
import argparse
from datetime import datetime
from typing import List, NamedTuple


# cmd: sudo timeout 5 strace -e trace=all -f -s 0 -yy -ttt -o strace.log -p 74132


class SyscallData(NamedTuple):
    pid: int
    timestamp: str
    syscall: str
    args: str
    retval: int

class SignalData(NamedTuple):
    pid: int
    timestamp: str
    signal_name: str
    si_signo: str
    si_code: str
    si_pid: int
    si_uid: int


def get_syscalls(content: str) -> List[SyscallData]:
    """extruct systemcall tuples from strace output 

    Args:
        content (str): strace output

    Returns:
        List[SyscallData]: commands as named tuple.
    """
    line_pattern = r'(\d+)\s+([\d.]+)\s+(\w+)\((.*?)\)\s+=\s+(.+)\n'
    matches = re.findall(line_pattern, content)
    
    result: List[SyscallData] = []
    for line in matches:
        pid = line[0]
        timestamp = line[1]
        syscall = line[2]
        args = line[3]
        retval = line[4]

        if "x" in retval:
            retval = int(retval, 16)
        else:
            retval = int(retval)
        
        result.append(SyscallData(pid=int(pid), timestamp=timestamp, syscall=syscall, args=args, retval=retval))
    
    return result

def get_signals(content: str) -> List[SignalData]:
    """Extract signal tuples from strace output

    Args:
        content (str): strace output

    Returns:
        List[SignalData]: signals as named tuple.
    """
    line_pattern = (
        r'(\d+)\s+([\d.]+)\s+---\s+(\w+)\s+\{si_signo=(\w+),\s+si_code=(\w+),\s+si_pid=(\d+),\s+si_uid=(\d+)\}'
    )
    matches = re.findall(line_pattern, content)

    result: List[SignalData] = []
    for line in matches:
        pid = int(line[0])
        timestamp = line[1]
        signal_name = line[2]
        si_signo = line[3]
        si_code = line[4]
        si_pid = int(line[5])
        si_uid = int(line[6])

        result.append(
            SignalData(
                pid=pid,
                timestamp=timestamp,
                signal_name=signal_name,
                si_signo=si_signo,
                si_code=si_code,
                si_pid=si_pid,
                si_uid=si_uid,
            )
        )

    return result


def combine_and_order_by_timestamp(syscalls: List[SyscallData], signals: List[SignalData]) -> List[dict]:
    # Convert namedtuples to dicts for easier handling
    combined = [s._asdict() for s in syscalls] + [s._asdict() for s in signals]
    # Sort by timestamp (convert to float for correct ordering)
    combined.sort(key=lambda x: float(x["timestamp"]))
    return combined


def append_to_json(json_file: str, syscalls: List[SyscallData], signals: List[SignalData]) -> None:
    
    data = []
    if os.path.exists(json_file):
        with open(json_file, "r") as f:
            data: List = json.load(f)
    else:
        os.makedirs(os.path.dirname(json_file), exist_ok=True)

    # Create a new entry with the current timestamp and counts
    new_data = {
        "timestamp": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        "syscall_count": len(syscalls),
        "signal_count": len(signals),
        "data" : combine_and_order_by_timestamp(syscalls, signals)
    }
    
    with open(json_file, "w") as f:        
        data.append(new_data)
        json.dump(data, f, indent=4)

    
if __name__ == "__main__":


    parser = argparse.ArgumentParser("strace_filter")
    parser.add_argument("-s", "--strace_file", type=str, required=True, help="strace log file with data to parse")
    parser.add_argument("-j", "--json_file", type=str, required=True, help="json file to save the data")
    args = parser.parse_args()


    with open(args.strace_file, 'r') as f:
        content = f.read()

    syscalls = get_syscalls(content)
    signals = get_signals(content)
    append_to_json(args.json_file, syscalls, signals)
