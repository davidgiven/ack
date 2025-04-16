from build.ab import export
from build.config import IS_WINDOWS

# This is the list of which plats to build.
PLATS = ([
    "linux386",
    "linux68k",
    "linuxmips",
    "linuxppc",
    "minix68k",
    "msdos386",
    "msdos86",
    "osx386",
    "osxppc",
    "pc86",
    "rpi",
    "pdpv7",
    "em22",
] + 
    # The i80 mach doesn't build on Windows because the ludicrous number of
    # object files blows the Windows command line limit.
    (["cpm"] if not IS_WINDOWS else []))

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
        [".+compiler", "examples+all"] + 
       ([] if IS_WINDOWS else [f"plat/{p}/tests" for p in TEST_PLATS if p in PLATS])
    ),
)
