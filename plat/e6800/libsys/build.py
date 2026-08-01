from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="e6800",
    srcs=(
        glob("plat/e6800/libsys/*.s")
        + glob("plat/e6800/libsys/*.c")
        + glob("plat/e6800/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/e6800/include"],
)

