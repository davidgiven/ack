from build import PLATS
from build.ab import export
from build.ack import ackcprogram

ackcprogram(name="hilo", plat="pc86", srcs=["./hilo.c"])
export(name="all", items={f"$(PLATIND)/examples/hilo": ".+hilo"})
