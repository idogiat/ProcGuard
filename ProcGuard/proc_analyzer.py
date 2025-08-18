import argparse
import json
import sys
import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from typing import NamedTuple, List

class SyscallData(NamedTuple):
    pid: int
    syscalls_count: int
    signals_count: int
    syscalls_data: List[pd.DataFrame]
    signals_data: List[pd.DataFrame]


def extruct_data_from_json(json_file: str, pid: int) -> SyscallData:
    """Extract syscall and signal data from a JSON file.

    Args:
        json_file (str): Path to the JSON file.

    Returns:
        SyscallData: Named tuple containing extracted data.
    """
    with open(json_file, "r") as f:
        data = json.load(f)

    syscalls = []
    signals = []
    syscalls_count = 0
    signals_count = 0

    for sample in data:
        syscalls_count += sample.get("syscall_count", 0)
        signals_count += sample.get("signal_count", 0)
        syscalls.append(pd.DataFrame(sample.get("syscalls", [])))
        signals.append(pd.DataFrame(sample.get("signals", [])))

    return SyscallData(pid=pid,
                       syscalls_count= syscalls_count,
                       signals_count=signals_count,
                       syscalls_data=syscalls,
                       signals_data=signals)


def analyze_data(syscall_data: SyscallData, training_data_file: str) -> int:
    """Analyze syscall and signal data to predict process status.

    Args:
        syscall_data (SyscallData): Extracted syscall and signal data.
        training_data_file (str): Path to the training data file.

    Returns:
        int: Return value indicating process status (0 for normal, -1 for suspicious).
    """
    # --- Load training data (labeled) ---
    training_df = pd.read_json(training_data_file)

    # --- Feature extraction from current process ---
    all_syscalls = pd.concat(syscall_data.syscalls_data, ignore_index=True)
    all_signals = pd.concat(syscall_data.signals_data, ignore_index=True)

    syscall_counts = all_syscalls['syscall'].value_counts() if not all_syscalls.empty else {}
    signal_counts = all_signals['signal_name'].value_counts() if not all_signals.empty else {}

    features = {
        'syscall_count': syscall_data.syscalls_count,
        'signal_count': syscall_data.signals_count,
        'mmap_count': syscall_counts.get('mmap', 0),
        'munmap_count': syscall_counts.get('munmap', 0),
        'write_count': syscall_counts.get('write', 0),
        'clock_nanosleep_count': syscall_counts.get('clock_nanosleep', 0),
        'gettid_count': syscall_counts.get('gettid', 0),
        'rt_sigprocmask_count': syscall_counts.get('rt_sigprocmask', 0),
        'rt_sigreturn_count': syscall_counts.get('rt_sigreturn', 0),
        'unique_signal_types': len(signal_counts),
        'dangerous_signal_count': sum(signal_counts.get(s,0) for s in ['SIGKILL','SIGSTOP','SIGUSR1'])
    }

    X_train = training_df.drop(columns=['label'])
    y_train = training_df['label']

    # --- Train RandomForest classifier ---
    clf = RandomForestClassifier(n_estimators=100, random_state=42)
    clf.fit(X_train, y_train)

    # --- Predict for current process ---
    X_test = pd.DataFrame([features])
    prediction = clf.predict(X_test)[0]

    print(f"PID: {syscall_data.pid}")
    print(f"Process status: {prediction}")
    retval = 0 if prediction == "normal" else 1
    return retval
    


if __name__ == "__main__":
    parser = argparse.ArgumentParser("strace_filter")
    parser.add_argument("-j", "--json_file", type=str, required=True, help="json strace file to process")
    parser.add_argument("-d", "--data", type=str, required=True, help="json file with data for model training")
    parser.add_argument("-p", "--process_id", type=int, required=True, help="process ID to filter")
    args = parser.parse_args()


    extruct_data = extruct_data_from_json(args.json_file, args.process_id)
    val = analyze_data(extruct_data, args.data)
    sys.exit(val)
    

    

    