from build.ab import simplerule, filenamesof, targetof
from build.c import hostcprogram, cprogram
from fnmatch import filter

hostcprogram(name="makeitems", srcs=["./makeitems.c"])

em_mnem = filter(filenamesof([targetof("modules/src/em_data")]), "*/em_mnem.h")[
    0
]
simplerule(
    name="itemtab_h",
    ins=[".+makeitems", em_mnem, "./itemtab.src"],
    outs=["=itemtab.h"],
    commands=["$[ins] > $[outs]"],
)

cprogram(
    name="ra",
    srcs=[
        "./ra_allocl.c",
        "./ra_aux.c",
        "./ra.c",
        "./ra_interv.c",
        "./ra_items.c",
        "./ra_lifet.c",
        "./ra_pack.c",
        "./ra_profits.c",
        "./ra_xform.c",
        ".+itemtab_h",
        "./ra_allocl.h",
        "./ra_aux.h",
        "./ra.h",
        "./ra_interv.h",
        "./ra_items.h",
        "./ra_lifet.h",
        "./ra_pack.h",
        "./ra_profits.h",
        "./ra_xform.h",
    ],
    deps={
        "util/ego/share",
        "modules/src/em_data",
        "h",
    },
    cflags=["-DVERBOSE", "-DNOTCOMPACT"],
)
