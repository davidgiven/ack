from build import PLATS
from build.ack import ackclibrary
from glob import glob

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        srcs=glob("mach/powerpc/libem/*.s"),
        plat=plat,
    )
