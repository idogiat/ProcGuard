import sqlite3
import subprocess
from datetime import datetime
from abc import ABC, abstractmethod
from typing import NamedTuple


DB_PATH = "ps_snapshot.db"
DB_NAME = "snapshots"


class DBMgr(ABC):

    def __init__(self, db_path) -> None:
        self.db_path = db_path
        self.connect()
        self._init_db()
        self.close()
    
    def connect(self) -> None:
        """
        Reconnect to the database.
        """
        self.conn = sqlite3.connect(self.db_path)
        self.cursor = self.conn.cursor()

    @abstractmethod
    def _init_db(self) -> None:
        """
        Initialize the database and create the snapshots table if it doesn't exist.
        """
        raise NotImplementedError("Subclasses must implement __init_db method")


    def close(self) -> None:
        """
        Close the database connection.
        """
        self.conn.close()
    
    def __del__(self):
        self.close()
    
    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()


class PSLine(NamedTuple):
    timestamp: str
    pid: int
    ppid: int
    comm: str
    etime: str
    cpu: float
    mem: float
    rss: int
    vsz: int
    stat: str
    flags: str


class DBMgrPS(DBMgr):
    PS_CMD = ["ps", "-eo", "pid,ppid,comm,etime,%cpu,%mem,rss,vsz,stat,flags ", "--sort=-%mem"]
    MAX_LINES = 2000
    PARTS_COUNT = 10
    def __init__(self, db_path=DB_PATH) -> None:
        super().__init__(db_path)
    
    def _init_db(self) -> None:
        self.cursor.execute('''
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
        self.conn.commit()
    
    def snapshot(self) -> None:
        result = subprocess.run(self.PS_CMD, stdout=subprocess.PIPE, text=True)
        lines = result.stdout.strip().split('\n')[1:]

        timestamp = datetime.now().strftime("%d-%m %H:%M")

        for line in lines:
            parts = line.split(None, self.PARTS_COUNT)

            if len(parts) == self.PARTS_COUNT :
                parts.insert(0, timestamp)  # Insert timestamp at the beginning
                psline = PSLine(*parts)
                self.cursor.execute('''
                INSERT INTO snapshots (
                    timestamp, pid, ppid, comm, etime, cpu, mem, rss, vsz, stat, flags
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', psline)

        self.conn.commit()

    def delete_old_snapshots(self) -> None:
        self.cursor.execute(f'''DELETE FROM snapshots WHERE id NOT IN (SELECT id FROM snapshots ORDER BY timestamp DESC LIMIT {self.MAX_LINES})''')
        self.conn.commit()
