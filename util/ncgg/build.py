from build.ab import simplerule, Targets, Rule
from build.c import cprogram, cppfile
from build.yacc import bison, flex
from glob import glob

bison(name="cggparser", src="./cgg.y")
flex(name="cgglexer", src="./scan.l")

simplerule(
    name="keywords",
    ins=[
        "./make_enterkeyw_c.lua",
        "./keywords",
    ],
    outs=["=enterkeyw.c"],
    commands=["$(LUA) $[ins[0]] < $[ins[1]] > $[outs]"],
)


cprogram(
    name="ncgg",
    srcs=(
        glob("util/ncgg/*.c")
        + glob("util/ncgg/*.h")
        + [".+cggparser", ".+cgglexer", ".+keywords"]
    ),
    deps=["h", "modules/src/em_data"],
)


@Rule
def ncgg(self, name, srcs: Targets = [], deps: Targets = [], cflags=[]):
    cpptable = cppfile(
        name=f"{self.localname}/cpptable", srcs=srcs, deps=deps, cflags=cflags
    )

    simplerule(
        replaces=self,
        ins=["util/ncgg", cpptable],
        outs=["=tables.c", "=tables.h"],
        commands=[
            "$[ins[0]] -o $[dir] $[ins[1:]]"
        ],
        label="NCGG",
    )
