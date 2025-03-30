from build.ab import export
from build.c import clibrary
from glob import glob
from build.ack import exportheaders

headers = glob("**/*.h", root_dir="plat/minix68k/include", recursive=True)

clibrary(
    name="include",
    hdrs={k: f"./{k}" for k in headers},
    deps=["plat/minix/include"],
)

export(
    name="all",
    items=exportheaders(".+include", prefix="$(PLATIND)/minix68k/include"),
    deps=["plat/minix/include+all"],
)
