from build.ab import Rule, Target, simplerule
from build.c import hostcprogram
from util.LLgen.build import llgen

llgen(
    name="llgen",
    srcs=[
        "./topgen.g",
    ],
)

hostcprogram(
    name="topgen",
    srcs=[
        ".+llgen",
        "./LLlex.c",
        "./hash.c",
        "./hash.h",
        "./main.c",
        "./misc.h",
        "./pattern.c",
        "./pattern.h",
        "./symtab.c",
        "./symtab.h",
        "./token.h",
        "./tunable.h",
    ],
)


@Rule
def topgen(self, name, src: Target):
    simplerule(
        replaces=self,
        ins=["util/topgen", src],
        outs=["=gen.c.h", "=gen.h"],
        commands=["$[ins] $[dir]", "mv $[dir]/gen.c $[outs[0]]"],
        label="TOPGEN",
    )
