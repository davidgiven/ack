from build.ab import export, simplerule
from build.c import cprogram, hostcprogram

hostcprogram(
    name="mktables",
    srcs=["./mktables.c"],
)

simplerule(
    name="tables",
    ins=[
        ".+mktables",
        "lib/descr/fe",
    ],
    outs=["=dmach.c", "=intable.c"],
    commands=["$[ins[0]] lib", "mv dmach.c intable.c $[dir]"],
)

cprogram(
    name="ack",
    srcs=[
        "./data.c",
        "./files.c",
        "./grows.c",
        "./list.c",
        "./main.c",
        "./rmach.c",
        "./run.c",
        "./scan.c",
        "./svars.c",
        "./trans.c",
        "./util.c",
        "./ack.h",
        "./data.h",
        "./dmach.h",
        "./grows.h",
        "./list.h",
        "./trans.h",
        ".+tables",
    ],
    deps=[
        "h",
        "modules/src/data",
        "modules/src/system",
    ],
)

export(
    name="all",
    items={
        "$(INSDIR)/bin/ack$(EXT)": ".+ack",
        "$(INSDIR)/share/man/man1/ack.1": "./ack.1.X",
        "$(PLATIND)/descr/fe": "lib/descr/fe",
    },
)
