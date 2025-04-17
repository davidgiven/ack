from build.ab import export
from build.c import clibrary
from glob import glob
from build.ack import exportheaders

headers = glob("**/*.h", root_dir="plat/pdpv7/include", recursive=True)

clibrary(
    name="include",
    hdrs={k: f"./{k}" for k in headers},
)

export(
    name="all",
    items=exportheaders(".+include", prefix="$(PLATIND)/pdpv7/include"),
)
