from build.ab import export
from build.c import clibrary
from build.ack import exportheaders
from glob import glob

headers = glob("**/*.h", root_dir="lang/cem/libcc.ansi/headers", recursive=True)

clibrary(name="headers", hdrs={k: f"./{k}" for k in headers})

export(
    name="all",
    items=exportheaders(
        "lang/cem/libcc.ansi/headers", prefix="$(PLATIND)/include/ansi"
    ),
)
