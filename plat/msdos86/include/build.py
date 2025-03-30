from build.ab import export
from build.c import clibrary
from glob import glob
from build.ack import exportheaders

headers = glob("**/*.h", root_dir="plat/msdos86/include", recursive=True)

clibrary(
    name="include",
    hdrs={k: f"./{k}" for k in headers},
    deps=["plat/msdos/include"],
)

export(
    name="all",
    items=exportheaders(".+include", prefix="$(PLATIND)/msdos86/include"),
    deps=["plat/msdos/include+all"],
)
