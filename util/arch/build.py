from build.ab import export
from build.c import cprogram

cprogram(
    name="aal",
    srcs=["./archiver.c"],
    deps=[
        "h",
        "modules/src/data",
        "modules/src/object",
        "modules/src/string",
        "modules/src/system",
    ],
    cflags=["-DAAL"],
)

export(
    name="all",
    items={
        "$(INSDIR)/bin/aal$(EXT)": ".+aal",
        "$(INSDIR)/share/man/man1/aal.1": "./aal.1",
    },
)
