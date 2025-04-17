from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="pdpv7",
    srcs=(
        glob("plat/pdpv7/libsys/*.s")
        + glob("plat/pdpv7/libsys/*.c")
        + glob("plat/pdpv7/libsys/*.h")
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/pdpv7/include"],
)
