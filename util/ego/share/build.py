from build.ab import simplerule, filenamesof, targetof
from build.c import clibrary, hostcprogram
from fnmatch import filter

hostcprogram(name="makecldef", srcs=["./makecldef.c"])

em_mnem = filter(filenamesof([targetof("modules/src/em_data")]), "*/em_mnem.h")[
    0
]
simplerule(
    name="classdefs_h",
    ins=[
        ".+makecldef",
        em_mnem,
        "./cldefs.src",
    ],
    outs=["=classdefs.h"],
    commands=["$[ins] > $[outs]"],
)

simplerule(
    name="pop_push_h",
    ins=[
        "./pop_push.awk",
        "h/em_table",
    ],
    outs=["=pop_push.h"],
    commands=["awk -f $[ins[0]] < $[ins[1]] > $[outs]"],
)

clibrary(
    name="share",
    srcs=[
        ".+classdefs_h",
        ".+pop_push_h",
        "./debug.c",
        "./global.c",
        "./files.c",
        "./go.c",
        "./map.c",
        "./utils.c",
        "./get.c",
        "./put.c",
        "./alloc.c",
        "./lset.c",
        "./cset.c",
        "./parser.c",
        "./stack_chg.c",
        "./locals.c",
        "./init_glob.c",
        "./alloc.h",
        "./cset.h",
        "./debug.h",
        "./def.h",
        "./files.h",
        "./get.h",
        "./global.h",
        "./go.h",
        "./init_glob.h",
        "./locals.h",
        "./lset.h",
        "./map.h",
        "./parser.h",
        "./put.h",
        "./stack_chg.h",
        "./types.h",
        "./utils.h",
    ],
    hdrs={
        f"ego/share/{k}": f"./{k}"
        for k in [
            "alloc.h",
            "cset.h",
            "debug.h",
            "def.h",
            "files.h",
            "get.h",
            "global.h",
            "go.h",
            "init_glob.h",
            "locals.h",
            "lset.h",
            "map.h",
            "parser.h",
            "put.h",
            "stack_chg.h",
            "types.h",
            "utils.h",
        ]
    },
    deps=[
        "h",
        "modules/src/em_data",
    ],
    cflags=["-DDEBUG", "-DVERBOSE", "-DNOTCOMPACT"],
)
