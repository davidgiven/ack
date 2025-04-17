from build.ab import export
from build.c import cprogram

cprogram(
    name="cvmach",
    srcs=["./cvmach.c"],
    deps=[
        "h",
        "modules/src/object",
    ],
)

export(name="all", items={"$(PLATDEP)/cvmach$(EXT)": ".+cvmach"})
