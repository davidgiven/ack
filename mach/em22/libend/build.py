from build import PLATS
from build.ack import ackclibrary

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        srcs=[
            "./edata.e",
            "./em_end.e",
            "./end.e",
            "./etext.e",
        ],
        plat=plat,
    )
