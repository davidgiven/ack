from build.ab import export

# This is the list of which plats to build.
PLATS = [
    "cpm",
#    "linux386",
#    "linux68k",
#    "linuxmips",
#    "linuxppc",
#    "minix68k",
#    "msdos386",
#    "msdos86",
#    "osx386",
#    "osxppc",
#    "pc86",
#    "rpi",
#    "pdpv7",
#    "em22",
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

export(name="all", deps=[".+compiler", "examples+all"])
