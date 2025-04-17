from build.ab import simplerule, Targets, Rule, filenamesof, targetof
from build.c import cprogram, cppfile, clibrary
from build.yacc import bison, flex
from glob import glob

bison(name="cggparser", src="./gram.y")
flex(name="cgglexer", src="./scan.l")

simplerule(
    name="ircodes",
    ins=["./ircodes.sh", "./ir.dat"],
    outs=["=ircodes-dyn.h", "=ircodes.c"],
    commands=["$[ins] $[outs]"],
)

clibrary(
    name="lib",
    srcs=[".+ircodes", "./ircodes.h"],
    hdrs={
        "ircodes.h": "./ircodes.h",
        "mcgg.h": "./mcgg.h",
        "ircodes-dyn.h": filenamesof([targetof(".+ircodes")])[0],
    },
)

cprogram(
    name="mcgg",
    srcs=(
        glob("util/mcgg/*.c")
        + glob("util/mcgg/*.h")
        + [".+cggparser", ".+cgglexer"]
    ),
    deps=[
        ".+lib",
        "h",
        "modules/h",
        "modules/src/em_data",
        "modules/src/data",
        "modules/src/system",
    ],
)


@Rule
def mcgg(self, name, srcs: Targets = [], deps: Targets = [], cflags=[]):
    cpptable = cppfile(
        name=f"{self.localname}/cpptable", srcs=srcs, deps=deps, cflags=cflags
    )

    simplerule(
        replaces=self,
        ins=["util/mcgg", cpptable],
        outs=["=tables.c", "=tables.h"],
        commands=["$[ins[0]] -i $[ins[1]] -o $[outs[0]] -h $[outs[1]]"],
        label="MCGG",
    )
