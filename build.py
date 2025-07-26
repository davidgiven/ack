from build.ab import export
from build.config import IS_WINDOWS
import os
from os.path import *
from glob import glob

_plats = os.getenv("PLATS")
if _plats == "all":
    PLATS = {dirname(p) for p in glob("*/build.py", root_dir="plat")}
else:
    PLATS = _plats.split(" ")
    
if IS_WINDOWS and ("cpm" in PLATS):
    print("Warning: the cpm plat can't be built on Windows because of reasons; skipping.")
    PLATS.discard("cpm")
print("Building plats: " + (" ".join(PLATS)))

# This is the list of which plats to test.
TEST_PLATS = [
    "cpm",
    "linux68k",
    "linuxppc",
    "pc86",
]

# This contains the platform-independent host tooling required to build the plats.
export(
    name="common",
    deps=[
        "lang/cem/cemcom.ansi+all",
        "lang/cem/cpp.ansi+all",
        "lang/basic/src+all",
        "lang/m2/comp+all",
        "lang/pc/comp+all",
        "util/ack+all",
        "util/amisc+all",
        "util/arch+all",
        "util/ego+all",
        "util/led+all",
        "util/misc+all",
        "util/opt+all",
    ],
)

export(
    name="compiler",
    deps=([".+common"] + [f"plat/{p}+all" for p in PLATS]),
)

export(
    name="all",
    deps=(
        [".+compiler", "examples+all"]
        + (
            []
            if IS_WINDOWS
            else [f"plat/{p}/tests" for p in TEST_PLATS if p in PLATS]
        )
    ),
)
