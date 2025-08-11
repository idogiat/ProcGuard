import re
import json
import os
from typing import List, NamedTuple, Dict
from pprint import pprint

# cmd: sudo timeout 5 strace -e trace=all -f -s 0 -yy -ttt -o strace.log -p 74132


class SyscallData(NamedTuple):
    pid: int
    timestamp: str
    syscall: str
    args: str
    retval: int


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
    
def append_to_json(json_file: str, content: List[SyscallData]) -> None:
    
    data = []
    if os.path.exists(json_file):
        with open(json_file, "r") as f:
            data: List = json.load(f)
    
    with open(json_file, "w") as f:        
        for x in content:
            data.append(x._asdict())
        json.dump(data, f, indent=4)
        
    
    
if __name__ == "__main__":
    content_file = "strace.log"
    json_file = "job.json"
    with open(content_file, 'r') as f:
        content = f.read()

    syscalls = get_syscalls(content)
    append_to_json(json_file, syscalls)    