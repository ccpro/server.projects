import sys
import os
import datetime
import time


def order_files(path) :
    print(path + "\n")

    files = os.listdir(path)

    dirs = []
    audio = []
    other = []

    d = datetime.datetime(1971, 9, 27, 0, 0)
    count = 0

    for f in files:
        p = os.path.join(path, f)

        if os.path.isdir(p):
            dirs.append(p)
        elif f.endswith(".mp3") or f.endswith(".flac") :
            audio.append(os.path.join(path, f))
        else :
            other.append(os.path.join(path, f))

    ts = time.mktime(d.timetuple())
    audio.sort()
    for a in audio:
        os.utime(a, (ts, ts))
        ts += 1
        print(a)

    other.sort()
    for o in other:
        os.utime(o, (ts, ts))
        ts += 1
        print(o)

    ts = time.mktime(d.timetuple())
    dirs.sort()
    for dr in dirs:
        os.utime(dr, (ts, ts))
        ts += 1
        print(dr)

    for dr in dirs:
        order_files(dr)

if len(sys.argv) > 1 :
    path = sys.argv[1]
if os.path.isdir(path) and os.path.exists(path) :
    order_files(path)
else :
    print("path '" + path + "' doesn't exist")
