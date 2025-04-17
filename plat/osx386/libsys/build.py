from build.ack import ackclibrary
from glob import glob

ackclibrary(
    name="libsys",
    plat="osx386",
    srcs=(
        glob("plat/osx386/libsys/*.s")
        + glob("plat/osx/libsys/*.c")
        + glob("plat/osx/libsys/*.s")
        + glob("plat/osx/libsys/*.h")
        + ["plat/linux/libsys/_hol0.s", "plat/linux386/libsys/trapno.s"]
    ),
    deps=["lang/cem/libcc.ansi/headers", "plat/osx386/include"],
)
