from build import PLATS
from build.ab import simplerule, export
from build.ack import ackclibrary, ackcfile
from util.cmisc.build import tabgen

for plat in PLATS:
    lib = ackclibrary(
        name=f"lib_{plat}",
        plat=plat,
        srcs=[
            "./abs.c",
            "./asc.c",
            "./asrt.c",
            "./atn.c",
            "./bc_io.h",
            "./chr.c",
            "./conversion.c",
            "./error.c",
            "./exp.c",
            "./fef.e",
            "./fif.e",
            "./file.c",
            "./hlt.c",
            "./io.c",
            "./lib.h",
            "./log.c",
            "./mki.c",
            "./oct.c",
            "./peek.c",
            "./power.c",
            "./print.c",
            "./random.c",
            "./read.c",
            "./return.c",
            "./salloc.c",
            "./setline.e",
            "./sgn.c",
            "./sin.c",
            "./sqt.c",
            "./stop.c",
            "./string.c",
            "./swap.c",
            "./trace.c",
            "./trap.c",
            "./write.c",
        ],
        deps=["lang/cem/libcc.ansi/headers"],
    )

    export(
        name=f"all_{plat}",
        items={
            f"$(PLATIND)/{plat}/libbasic.a": f".+lib_{plat}",
        },
    )
