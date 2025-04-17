from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="linux386",
    srcs=(
        glob("plat/linux386/libsys/*.s")
        + glob("plat/linux/libsys/*.c")
        + glob("plat/linux/libsys/*.s")
        + glob("plat/linux/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/linux386/include"],
)
