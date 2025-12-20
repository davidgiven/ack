from build.ab import simplerule
from build.c import clibrary
from glob import glob


simplerule(
    name="em_codeek_h",
    ins=[
        "./make_codeek_h.lua",
        "h/em_table_lib.lua",
        "h/em_table",
        "./em.nogen",
    ],
    outs=["=em_codeEK.h"],
    commands=[
        "$(LUA) $[ins[0]] < $[ins[2]] > $[outs]",
        "cat $[ins[3]] >> $[outs]",
    ],
)

clibrary(
    name="headers",
    hdrs={
        "em_private.h": "./em_private.h",
        "em_code.h": "./em_code.h",
        "em_codeCE.h": "./em_codeCE.h",
        "em_codeO.h": "./em_codeO.h",
        "em_codeEK.h": ".+em_codeek_h",
    },
)


def build_variant(name, cflags):
    clibrary(
        name=name,
        srcs=[
            "./C_out.c",
            "./bhcst.c",
            "./bhdlb.c",
            "./bhdnam.c",
            "./bhfcon.c",
            "./bhicon.c",
            "./bhilb.c",
            "./bhpnam.c",
            "./bhucon.c",
            "./crcst.c",
            "./crdlb.c",
            "./crdnam.c",
            "./crilb.c",
            "./crpnam.c",
            "./crscon.c",
            "./crxcon.c",
            "./cst.c",
            "./dfdlb.c",
            "./dfdnam.c",
            "./dfilb.c",
            "./dlb.c",
            "./dnam.c",
            "./em.c",
            "./end.c",
            "./endarg.c",
            "./exc.c",
            "./failed.c",
            "./fcon.c",
            "./getid.c",
            "./icon.c",
            "./ilb.c",
            "./insert.c",
            "./insert.h",
            "./internerr.c",
            "./msend.c",
            "./msstart.c",
            "./op.c",
            "./opcst.c",
            "./opdlb.c",
            "./opdnam.c",
            "./opilb.c",
            "./opnarg.c",
            "./oppnam.c",
            "./pnam.c",
            "./pro.c",
            "./pronarg.c",
            "./psdlb.c",
            "./psdnam.c",
            "./pspnam.c",
            "./scon.c",
            "./ucon.c",
            "modules/src/read_em/em_comp.h",
        ],
        deps=[
            ".+headers",
            "h",
            "modules/h",
            "modules/src/em_data",
            "modules/src/system",
            "modules/src/alloc",
        ],
        cflags=cflags
    )


build_variant("lib_e", ["-DREADABLE_EM"])
build_variant("lib_k", [])
