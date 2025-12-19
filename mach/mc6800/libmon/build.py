from build import PLATS
from build.ack import ackclibrary

for plat in PLATS:
    ackclibrary(
        name=f"lib_{plat}",
        srcs=[
            "./head_em.s",
        ],
        plat=plat,
    )
