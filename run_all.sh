#!/bin/bash
# filepath: /home/ido/dev/linux_internals/run_all.sh

SCANNER="./scanner.py"
TARGET1="./build/targets/MsgHandler"
TARGET2="./build/targets/AnotherTarget" # Replace with your actual second target binary
PID_FILE="./procguard_pids.txt"

run() {
    # Build all targets
    make -C ProcGuard

    # Start scanner.py in background
    python3 $SCANNER &
    SCAN_PID=$!
    echo "Started scanner.py with PID $SCAN_PID"
    echo $SCAN_PID > $PID_FILE

    # Start MsgHandler in background
    $TARGET1 &
    T1_PID=$!
    echo "Started MsgHandler with PID $T1_PID"
    echo $T1_PID >> $PID_FILE

    # Start second target in background
    $TARGET2 &
    T2_PID=$!
    echo "Started AnotherTarget with PID $T2_PID"
    echo $T2_PID >> $PID_FILE
}

kill_all() {
    if [ -f $PID_FILE ]; then
        while read pid; do
            if kill -0 $pid 2>/dev/null; then
                kill -9 $pid
                echo "Killed PID $pid"
            fi
        done < $PID_FILE
        rm -f $PID_FILE
    else
        echo "No PID file found. Nothing to kill."
    fi
}

case "$1" in
    run)
        run
        ;;
    kill)
        kill_all
        ;;
    *)
        echo "Usage: $0 {run|kill}"
        exit 1
        ;;
esac