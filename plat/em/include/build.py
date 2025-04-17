from build.ab import export
from build.c import clibrary
from build.ack import exportheaders
from glob import glob

headers = glob("**/*.h", root_dir="plat/em/include", recursive=True)

clibrary(name="include", hdrs={k: f"./{k}" for k in headers})
export(
    name="all",
    items=exportheaders(".+include", prefix="$(PLATIND)/em/include"),
)
