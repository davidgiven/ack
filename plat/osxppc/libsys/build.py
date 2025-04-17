from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="osxppc",
    srcs=(
        glob("plat/osxppc/libsys/*.s")
        + glob("plat/osx/libsys/*.c")
        + glob("plat/osx/libsys/*.s")
        + glob("plat/osx/libsys/*.h")
        + ["plat/linux/libsys/_hol0.s"]
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/osxppc/include"],
)
