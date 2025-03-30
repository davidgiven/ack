from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="em22",
    srcs=(
        glob("plat/em/libsys/*.c")
        + glob("plat/em/libsys/*.e")
        + glob("plat/em/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/em/include", "h"],
)
