from build.ab import export
from build.ack import exportheaders, clibrary
from glob import glob

headers = glob("**/*.h", root_dir="plat/osx/include", recursive=True)

clibrary(name="include", hdrs={k: f"./{k}" for k in headers})

export(
    name="all",
    items=exportheaders(".+include", prefix="$(PLATIND)/osx/include"),
)
