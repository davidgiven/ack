from build.ack import ackclibrary
from build.c import clibrary
from glob import glob

clibrary(name="headers", hdrs={"libsysasm.h": "./libsysasm.h"})

ackclibrary(
    name="libsys",
    plat="msdos386",
    srcs=(
        glob("plat/msdos386/libsys/*.s")
        + glob("plat/msdos386/libsys/*.c")
        + glob("plat/msdos386/libsys/*.h")
        + glob("plat/msdos/libsys/*.c")
        + glob("plat/msdos/libsys/*.s")
        + glob("plat/msdos/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/msdos386/include"],
)
