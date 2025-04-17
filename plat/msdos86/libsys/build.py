from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="msdos86",
    srcs=(
        glob("plat/msdos86/libsys/*.s")
        + glob("plat/msdos86/libsys/*.c")
        + glob("plat/msdos86/libsys/*.h")
        + glob("plat/msdos/libsys/*.c")
        + glob("plat/msdos/libsys/*.s")
        + glob("plat/msdos/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/msdos86/include"],
)
