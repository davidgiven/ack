from build.ab import export, simplerule
from build.c import cprogram
from glob import glob
from util.LLgen.build import llgen
from util.cmisc.build import tabgen

str_roots = [
    "code",
    "declar",
    "def",
    "estack",
    "expr",
    "field",
    "idf",
    "l_brace",
    "l_outdef",
    "l_state",
    "macro",
    "next",
    "proto",
    "stack",
    "stmt",
    "struct",
    "switch",
    "type",
    "util",
]

simplerule(
    name="parameters",
    ins=["./BigPars"],
    outs=["=parameters.h"],
    commands=[
        "echo '#ifndef PARAMETERS_H' > $[outs]",
        "echo '#define PARAMETERS_H' >> $[outs]",
        "grep -v '^!' < $[ins] >> $[outs]",
        "echo '#endif' >> $[outs]",
    ],
)

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
        "./declar.g",
        "./expression.g",
        "./ival.g",
        "./program.g",
        "./statement.g",
    ],
)

tabgen(name="tabgen", src="./char.tab")

cprogram(
    name="cemcom",
    srcs=(
        glob("lang/cem/cemcom.ansi/*.c")
        + glob("lang/cem/cemcom.ansi/*.h")
        + strtargets
        + [".+parameters", ".+llgen", ".+next-c", ".+symbol2str-c", ".+tabgen"]
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

export(name="all", items={"$(PLATDEP)/em_cemcom.ansi$(EXT)": ".+cemcom"})
