from build import PLATS
from build.ack import ackclibrary

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        srcs=[
            "./edata.s",
            "./em_end.s",
            "./end.s",
            "./etext.s",
        ],
        plat=plat,
    )
