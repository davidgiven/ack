from build.ab import export, simplerule
from build.c import cprogram, hostcprogram

hostcprogram(
    name="maktab",
    srcs=["./maktab.c"],
    deps=[
        "h",
        "modules/src/em_data",
    ],
)

simplerule(
    name="asstb",
    ins=[".+maktab", "etc/ip_spec.t"],
    outs=["=asstb.c"],
    commands=["$[ins] $[outs]"],
)

cprogram(
    name="ass",
    srcs=[
        "./ass00.c",
        "./ass30.c",
        "./ass40.c",
        "./ass50.c",
        "./ass60.c",
        "./ass70.c",
        "./ass80.c",
        "./assci.c",
        "./asscm.c",
        "./assda.c",
        "./assrl.c",
        "./ass00.h",
        "./assci.h",
        "./asscm.h",
        "./assex.h",
        "./assrl.h",
        ".+asstb",
    ],
    deps=[
        "h",
        "modules/src/em_data",
        "modules/src/data",
        "modules/src/system",
    ],
)

export(
    name="all",
    items={
        "$(PLATDEP)/em_ass$(EXT)": ".+ass",
        "$(INSDIR)/share/man/man6/em_ass.6": "./em_ass.6",
    },
)
