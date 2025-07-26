from build.ab import export
from build.c import cprogram
from glob import glob

cprogram(
    name="led",
    srcs=glob("util/led/*.c") + glob("util/led/*.h"),
    deps=[
        "h",
        "modules/src/data",
        "modules/src/object",
        "modules/src/string",
        "modules/src/system",
    ],
)

export(
    name="all",
    items={
        "$(PLATDEP)/em_led$(EXT)": ".+led",
        "$(INSDIR)/share/man/man6/led.6": "./led.6",
    },
)
