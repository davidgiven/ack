from build.ab import simplerule, export, filenamesof
from build.c import cprogram
from build.utils import filenamesmatchingof
from util.LLgen.build import llgen

lfiles = llgen(name="llgen", srcs=["./basic.g"])

simplerule(
    name="tokentab_h",
    ins=["./maketokentab", filenamesmatchingof([lfiles], "*.h")],
    outs=["=tokentab.h"],
    commands=["$[ins[0]] < $[ins[1]] > $[outs]"],
)

cprogram(
    name="em_bem",
    srcs=[
        ".+llgen",
        ".+tokentab_h",
        "./basic.lex",
        "./bem.c",
        "./bem.h",
        "./compile.c",
        "./eval.c",
        "./eval.h",
        "./func.c",
        "./func.h",
        "./gencode.c",
        "./gencode.h",
        "./graph.c",
        "./graph.h",
        "./initialize.c",
        "./llmess.h",
        "./parsepar.c",
        "./parsepar.h",
        "./symbols.c",
        "./symbols.h",
        "./util.c",
        "./util.h",
        "./yylexp.h",
    ],
    deps=[
        "modules/h",
        "modules/src/system",
        "modules/src/em_code+lib_k",
        "modules/src/em_mes",
    ],
)


export(name="all", items={"$(PLATDEP)/em_bem$(EXT)": ".+em_bem"})
