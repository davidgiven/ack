from build.ab import export, simplerule
from build.c import cprogram
from glob import glob
from util.LLgen.build import llgen
from util.cmisc.build import tabgen

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
        "./program.g",
        "./statement.g",
    ],
)

tabgen(name="tabgen", src="./char.tab")

cprogram(
    name="em_m2",
    srcs=(
        glob("lang/m2/comp/*.c")
        + glob("lang/m2/comp/*.h")
        + [".+parameters", ".+llgen", ".+symbol2str-c", ".+tabgen"]
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
        "lang/m2/include",
    ],
)

export(
    name="all",
    items={
        "$(PLATDEP)/em_m2$(EXT)": ".+em_m2",
        "$(INSDIR)/share/man/man6/em_m2.6": "./em_m2.6",
    },
)
