import sqlite3
import subprocess
from datetime import datetime
from abc import ABC, abstractmethod
from typing import List, NamedTuple, Optional
from functools import partialmethod
from copy import copy


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
    PS_CMD = ["ps", "-eo", "pid,ppid,comm,etime,%cpu,%mem,rss,vsz,stat,flags"]
    MAX_LINES = 4000
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
    
    def _get_ps_cmd_output(self, filter_by: str, max_results: Optional[int] = None) -> List[str]:
        """
        Execute a command and return its output as a list of strings.
        
        :param filter_by: String to filter the output lines.
        :param max_results: Maximum number of results to return. If None, return all results.
        :return: List of output lines containing the filter string.
        """
        cmd = copy(self.PS_CMD)
        cmd.append(f"--sort=-{filter_by}")

        if max_results:
            cmd.append(f"| head -n {max_results}")
        

        result = subprocess.run(" ".join(cmd), shell=True, text=True, capture_output=True)
        lines = result.stdout.splitlines()
        return lines[1:]
    
    get_ps_max_by_cpu = partialmethod(_get_ps_cmd_output, filter_by="%cpu", max_results=20)
    get_ps_max_by_mem = partialmethod(_get_ps_cmd_output, filter_by="%mem", max_results=20)
    get_ps_max_by_rss = partialmethod(_get_ps_cmd_output, filter_by="rss", max_results=20)
    get_ps_max_by_vsz = partialmethod(_get_ps_cmd_output, filter_by="vsz", max_results=20)

    def snapshot(self) -> None:
        """
        Take a snapshot of the current processes and store them in the database.
        This method retrieves the top processes by CPU, memory, and RSS usage,
        combines the results, and inserts them into the snapshots table with a timestamp.
        """
        lines = self.get_ps_max_by_cpu()
        lines += self.get_ps_max_by_mem()
        lines += self.get_ps_max_by_rss()
    
        timestamp = datetime.now().strftime("%d-%m %H:%M")

        for line in lines:
            parts = line.split(None, self.PARTS_COUNT)

            if len(parts) == self.PARTS_COUNT :
                parts.insert(0, timestamp)  # Insert timestamp at the beginning
                psline = PSLine(*parts)
                print(psline)
                self.cursor.execute('''
                INSERT INTO snapshots (
                    timestamp, pid, ppid, comm, etime, cpu, mem, rss, vsz, stat, flags
                ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', psline)

        self.conn.commit()

    def delete_old_snapshots(self) -> None:
        self.cursor.execute(f'''DELETE FROM snapshots WHERE id NOT IN (SELECT id FROM snapshots ORDER BY timestamp DESC LIMIT {self.MAX_LINES})''')
        self.conn.commit()
