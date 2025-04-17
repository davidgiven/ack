from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="linuxmips",
    srcs=(
        glob("plat/linuxmips/libsys/*.s")
        + glob("plat/linux/libsys/*.c")
        + glob("plat/linux/libsys/*.s")
        + glob("plat/linux/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/linuxmips/include"],
)
