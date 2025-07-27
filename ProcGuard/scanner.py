import sqlite3
import time
import subprocess
from datetime import datetime


PS_CMD = ["ps", "-eo", "pid,ppid,comm,etime,%cpu,%mem,rss,vsz,stat,flags ", "--sort=-%mem"]
PARTS_COUNT = 10
DB_PATH = "ps_snapshot.db"
DB_NAME = "snapshots"
MAX_LINES = 2000


def init_db():
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()

    cursor.execute('''
        CREATE TABLE IF NOT EXISTS snapshots (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            timestamp TEXT,
            pid INTEGER,
            ppid INTEGER,
            comm TEXT,
            etime TEXT,
            cpu REAL,
            mem REAL,
            rss INTEGER,
            vsz INTEGER,
            stat TEXT,
            flags TEXT
        )
    ''')
    conn.commit()


def get_top_process():
    """
    """
    result = subprocess.run(PS_CMD, stdout=subprocess.PIPE, text=True)
    lines = result.stdout.strip().split('\n')[1:]

    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    timestamp = datetime.now().strftime("%d-%m %H:%M")

    for line in lines:
        parts = line.split(None, PARTS_COUNT)
        print(parts)

        if len(parts) == PARTS_COUNT:
            pid, ppid, comm, etime, cpu, mem, rss, vsz, stat, flags = parts
            cursor.execute('''
            INSERT INTO snapshots (
                timestamp, pid, ppid, comm, etime, cpu, mem, rss, vsz, stat, flags
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
            ''', (timestamp, pid, ppid, comm, etime, cpu, mem, rss, vsz, stat, flags))

    conn.commit()

    cursor.execute(f'''DELETE FROM snapshots WHERE id NOT IN (SELECT id FROM snapshots ORDER BY timestamp DESC LIMIT {MAX_LINES})''')
    conn.commit()
    conn.close()


if __name__ == "__main__":
    init_db()
    while(True):
        get_top_process()
        time.sleep(5)