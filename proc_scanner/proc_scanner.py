from DBMgr import DBMgrProc
from time import sleep


if __name__ == "__main__":
    count = 1

    while(True):
        with DBMgrProc() as db:
            db.snapshot()
            db.delete_old_snapshots()
            print(f"Snapshot {count} taken.")
            count += 1
        sleep(5)