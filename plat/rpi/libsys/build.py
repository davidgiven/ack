from build.ab import simplerule
from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="rpi",
    srcs=(
        glob("plat/rpi/libsys/*.c")
        + glob("plat/rpi/libsys/*.h")
        + glob("plat/rpi/libsys/*.s")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/rpi/include"],
)
