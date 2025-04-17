from build.ab import export, simplerule
from build.c import cprogram
from glob import glob
from util.LLgen.build import llgen
from util.cmisc.build import tabgen
from os.path import *

simplerule(
    name="parameters",
    ins=["./make.parameters", "./Parameters"],
    outs=["=parameters.h"],
    commands=["sh $[ins[0]] < $[ins[1]] > $[outs]"],
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

xrules = []

for f in ["./def.xh", "./desig.xh", "./node.xh", "./scope.xh", "./type.xh"]:
    name = splitext(basename(f))[0]
    xrules += [
        simplerule(
            name=f"{name}_h",
            ins=["./make.allocd", f],
            outs=[f"={name}.h"],
            commands=["$[ins[0]] < $[ins[1]] > $[outs]"],
        )
    ]

for f in ["./casestat.xc", "./tmpvar.xc"]:
    name = splitext(basename(f))[0]
    xrules += [
        simplerule(
            name=f"{name}_c",
            ins=["./make.allocd", f],
            outs=[f"={name}.c"],
            commands=["$[ins[0]] < $[ins[1]] > $[outs]"],
        )
    ]

simplerule(
    name="next-c",
    ins=(
        ["./make.next"] + glob("lang/pc/comp/*.xh") + glob("lang/pc/comp/*.xc")
    ),
    outs=["=next.c"],
    commands=["sh $[ins] > $[outs]"],
)

llgen(
    name="llgen",
    srcs=[
        ".+tokenfile-g",  # must be first
        "./declar.g",
        "./expression.g",
        "./program.g",
        "./statement.g",
    ],
)

tabgen(name="tabgen", src="./char.tab")

cprogram(
    name="em_pc",
    srcs=(
        glob("lang/pc/comp/*.c")
        + glob("lang/pc/comp/*.h")
        + [
            ".+parameters",
            ".+llgen",
            ".+next-c",
            ".+symbol2str-c",
            ".+tabgen",
            xrules,
        ]
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
        "lang/pc/include",
    ],
)

export(name="all", items={"$(PLATDEP)/em_pc$(EXT)": ".+em_pc"})
