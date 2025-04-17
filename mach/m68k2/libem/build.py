from build import PLATS
from build.ack import ackclibrary
from glob import glob

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        srcs=(glob("mach/m68k2/libem/*.s") + glob("mach/m68k2/libem/*.c")),
        plat=plat,
        deps=["h"],
    )
