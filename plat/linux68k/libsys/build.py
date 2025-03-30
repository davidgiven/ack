from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="linux68k",
    srcs=(
        glob("plat/linux68k/libsys/*.s")
        + glob("plat/linux/libsys/*.c")
        + glob("plat/linux/libsys/*.s")
        + glob("plat/linux/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/linux68k/include"],
)
