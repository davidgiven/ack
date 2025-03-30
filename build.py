from build.ab import export

PLATS = [
    "pc86",
    "cpm",
    "linux386",
    "linux68k",
    "linuxppc",
    "linuxmips",
    "minix68k",
    "msdos86",
    "msdos386",
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
    deps=(
        [
            ".+common",
        ]
        + [f"plat/{p}+all" for p in PLATS]
    ),
)

export(name="all", deps=[".+compiler", "examples+all"])
