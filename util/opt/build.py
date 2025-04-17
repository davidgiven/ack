from build.ab import export, simplerule
from build.c import cprogram, hostcprogram
from build.yacc import bison, flex
from glob import glob

bison(name="bison", src="./mktab.y")

flex(name="flex", src="./scan.l")

hostcprogram(
    name="mktab",
    srcs=[
        ".+bison",
        ".+flex",
        "./param.h",
        "./types.h",
        "./pattern.h",
        "./optim.h",
    ],
    deps=["modules/src/em_data"],
)

simplerule(
    name="pattern_c",
    ins=[".+mktab", "./patterns", "lang/cem/cpp.ansi"],
    outs=["=pattern.c"],
    commands={"$[ins[2]] < $[ins[1]] | $[ins[0]] > $[outs]"},
)

simplerule(
    name="pop_push_c",
    ins=["./pop_push.awk", "h/em_table"],
    outs=["=pop_push.c"],
    commands=["awk -f $[ins[0]] < $[ins[1]] > $[outs]"],
)


def build_variant(name, cflags):
    cprogram(
        name=name,
        srcs=[
            ".+pattern_c",
            ".+pop_push_c",
            "./alloc.c",
            "./backward.c",
            "./cleanup.c",
            "./flow.c",
            "./getline.c",
            "./lookup.c",
            "./main.c",
            "./peephole.c",
            "./process.c",
            "./putline.c",
            "./reg.c",
            "./special.c",
            "./tes.c",
            "./util.c",
            "./var.c",
            "./alloc.h",
            "./ext.h",
            "./getline.h",
            "./line.h",
            "./lookup.h",
            "./optim.h",
            "./param.h",
            "./pattern.h",
            "./pop_push.h",
            "./proinf.h",
            "./putline.h",
            "./reg.h",
            "./tes.h",
            "./types.h",
            "./util.h",
        ],
        cflags=cflags,
        deps=["h", "modules/h", "modules/src/em_data", "modules/src/system"],
    )


build_variant("em_opt", [])
build_variant("em_opt2", ["-DGLOBAL_OPT"])

export(
    name="all",
    items={
        "$(PLATDEP)/em_opt$(EXT)": ".+em_opt",
        "$(PLATDEP)/em_opt2$(EXT)": ".+em_opt2",
        "$(INSDIR)/share/man/man6/em_opt.6": "./em_opt.6",
    },
)
