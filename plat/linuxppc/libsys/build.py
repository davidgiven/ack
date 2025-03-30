from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="linuxppc",
    srcs=(
        glob("plat/linuxppc/libsys/*.s")
        + glob("plat/linux/libsys/*.c")
        + glob("plat/linux/libsys/*.s")
        + glob("plat/linux/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/linuxppc/include"],
)
