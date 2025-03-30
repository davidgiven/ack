from build.ab import simplerule, Targets, Rule
from build.c import cprogram, cppfile
from build.yacc import bison, flex
from glob import glob

bison(name="cggparser", src="./bootgram.y")
flex(name="cgglexer", src="./bootlex.l")

cprogram(
    name="cgg",
    srcs=(["./main.c", "./booth.h", ".+cggparser", ".+cgglexer"]),
    deps=["h", "modules/src/em_data"],
)


@Rule
def cgg(self, name, srcs: Targets = [], deps: Targets = [], cflags=[]):
    cpptable = cppfile(
        name=f"{self.localname}/cpptable", srcs=srcs, deps=deps, cflags=cflags
    )

    simplerule(
        replaces=self,
        ins=["util/cgg", cpptable],
        outs=["=tables.c", "=tables.h"],
        commands=["$[ins]", "mv tables.c tables.h $[dir]"],
        label="CGG",
    )
