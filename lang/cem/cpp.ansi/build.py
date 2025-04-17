from build.ab import export, simplerule
from build.c import cprogram
from glob import glob
from util.LLgen.build import llgen
from util.cmisc.build import tabgen

str_roots = [
    "macro",
    "replace",
]

strtargets = [
    simplerule(
        name=f"allocd_{str}",
        ins=["./make.allocd", f"./{str}.str"],
        outs=[f"={str}.h"],
        commands=["$[ins[0]] < $[ins[1]] > $[outs]"],
    )
    for str in str_roots
]

simplerule(
    name="next-c",
    ins=[
        "./make.next",
    ]
    + [f"./{k}.str" for k in str_roots],
    outs=["=next.c"],
    commands=["$[ins] > $[outs]"],
)

simplerule(
    name="tokenfile-g",
    ins=[
        "./make.tokfile",
        "./tokenname.c",
    ],
    outs=["=tokenfile.g"],
    commands=["sh $[ins[0]] < $[ins[1]] > $[outs]"],
)

simplerule(
    name="symbol2str-c",
    ins=[
        "./make.tokcase",
        "./tokenname.c",
    ],
    outs=["=symbol2str.c"],
    commands=["sh $[ins[0]] < $[ins[1]] > $[outs]"],
)

llgen(
    name="llgen",
    srcs=[
        ".+tokenfile-g",  # must be first
        "./expression.g",
    ],
)

tabgen(name="tabgen", src="./char.tab")

cprogram(
    name="cpp.ansi",
    srcs=(
        glob("lang/cem/cpp.ansi/*.c")
        + glob("lang/cem/cpp.ansi/*.h")
        + strtargets
        + [".+llgen", ".+next-c", ".+symbol2str-c", ".+tabgen"]
    ),
    deps=[
        "h",
        "modules/h",
        "modules/src/system",
        "modules/src/alloc",
        "modules/src/em_code+lib_k",
        "modules/src/em_data",
        "modules/src/em_mes",
        "modules/src/flt_arith",
        "modules/src/idf",
        "modules/src/input",
        "modules/src/string",
        "modules/src/system",
    ],
)

export(name="all", items={"$(PLATDEP)/cpp.ansi$(EXT)": ".+cpp.ansi"})
