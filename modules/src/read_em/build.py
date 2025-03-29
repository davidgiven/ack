from build.ab import simplerule
from build.c import clibrary

simplerule(
    name="c_mnem_narg_h",
    ins=[
        "./make_C_mnem_narg_h.lua",
        "h/em_table_lib.lua",
        "h/em_table",
    ],
    outs=["=C_mnem_narg.h"],
    commands=["$(LUA) $[ins[0]] < $[ins[2]] > $[outs]"],
)

simplerule(
    name="c_mnem_h",
    ins=[
        "./make_C_mnem_h.lua",
        "h/em_table_lib.lua",
        "h/em_table",
    ],
    outs=["=C_mnem.h"],
    commands=["$(LUA) $[ins[0]] < $[ins[2]] > $[outs]"],
)


def build_variant(name, cflags):
    clibrary(
        name=name,
        cflags=(
            cflags
            + [
                "-DPRIVATE=static",
                "-DEXPORT=",
                "-DNDEBUG",
                "-DCHECKING",
            ]
        ),
        srcs=[
            ".+c_mnem_narg_h",
            ".+c_mnem_h",
            "./EM_vars.c",
            "./read_em.c",
            "./mkcalls.c",
        ],
        hdrs={"em_comp.h": "./em_comp.h"},
        deps=[
            "./reade.c",
            "./readk.c",
            "h",
            "modules/h",
            "modules/src/alloc",
            "modules/src/em_code+headers",
            "modules/src/em_data",
            "modules/src/string",
            "modules/src/system",
        ],
    )


build_variant("lib_ev", [])
build_variant("lib_kv", ["-DCOMPACT"])
