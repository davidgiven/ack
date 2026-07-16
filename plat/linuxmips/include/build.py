from build.ab import export
from build.ack import exportheaders, clibrary
from glob import glob

headers = glob("**/*.h", root_dir="plat/linuxmips/include", recursive=True)

clibrary(name="include", hdrs={k: f"./{k}" for k in headers}, deps=["plat/linux/include"])

export(
    name="all",
    items=exportheaders(".+include", prefix="$(PLATIND)/linuxmips/include"),
    deps=["plat/linux/include+all"],
)
