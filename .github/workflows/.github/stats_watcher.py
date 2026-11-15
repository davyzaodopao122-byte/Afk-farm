# stats_watcher.py — Python 3.10+ · Windows · conta partidas via replay
# Roda em segundo plano. Escreve stats.json que o menu lê.

import json
import time
import glob
from pathlib import Path

REPLAY_DIR = Path.home() / "AppData/Local/FortniteGame/Saved/Demos"
STATS_FILE = Path("stats.json")

seen = set()
stats = {"chests": 0, "matches": 0}

if STATS_FILE.exists():
    try:
        stats = json.loads(STATS_FILE.read_text())
    except Exception:
        pass

while True:
    if REPLAY_DIR.exists():
        for f in glob.glob(str(REPLAY_DIR / "*.replay")):
            p = Path(f)
            if p in seen:
                continue
            seen.add(p)
            stats["matches"] += 1
    STATS_FILE.write_text(json.dumps(stats))
    time.sleep(5)
