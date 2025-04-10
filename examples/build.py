from build import PLATS
from build.ab import export
from build.ack import ackcprogram
from os.path import *

ALL = [
    "hilo.bas",
    "hilo.c",
    "hilo.mod",
    "hilo.p",
    "mandelbrot.c",
    "paranoia.c",
    "startrek.c",
]
PROGRAMS = {
    "cpm": ALL,
    "linux386": ALL,
    "linux68k": ALL,
    "linuxmips": ALL,
    "linuxppc": ALL,
    "minix68k": ALL,
    "msdos386": ALL,
    "msdos86": ALL,
    "osx386": ALL,
    "osxppc": ALL,
    "pc86": ALL,
    "pdpv7": ALL,
    "rpi": [
        # The rpi plat only supports C.
        "hilo.c",
        "mandelbrot.c",
        "paranoia.c",
        "startrek.c",
    ],
    "em22": [
        "hilo.bas",
        "hilo.c",
        "hilo.mod",
        # Pascal doesn't work in em22 yet; AFAICT ass doesn't allow you to
        # have multiple libraries defining the same symbol like led does, so
        # some libc symbols conflict with libpc symbols.
        # "hilo.p",
        "mandelbrot.c",
        "paranoia.c",
        "startrek.c",
    ],
}

exports = {}
for plat, progs in PROGRAMS.items():
    if plat in PLATS:
        ackcprogram(
            name=f"dhrystone_{plat}",
            plat=plat,
            lang="c",
            srcs=["./dhry.h", "./dhry_1.c", "./dhry_2.c"],
            cflags=["-O3", "-DTIME"],
        )
        exports[f"$(PLATIND)/examples/dhrystone_{plat}.exe"] = f".+dhrystone_{plat}"

        for prog in progs:
            fullname, *args = prog.split(",")
            name = fullname.replace(".", "_")
            lang = splitext(fullname)[1][1:]
            exports[f"$(PLATIND)/examples/{fullname}"] = f"./{fullname}"
            ackcprogram(
                name=f"{name}_{plat}",
                plat=plat,
                lang=lang,
                srcs=[f"./{fullname}"],
                cflags=["-O2"] + args,
                ldflags=args,
            )
            exports[f"$(PLATIND)/examples/{name}_{plat}.exe"] = (
                f".+{name}_{plat}"
            )

export(name="all", items=exports)
