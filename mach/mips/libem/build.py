from build import PLATS
from build.ack import ackclibrary
from glob import glob

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        srcs=glob("mach/mips/libem/*.s"),
        plat=plat,
    )
