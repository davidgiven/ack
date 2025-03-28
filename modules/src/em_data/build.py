from build.ab import simplerule
from build.c import clibrary

genfiles = ["flag.c", "mnem.c", "mnem.h", "pseu.c", "pseu.h", "spec.h"]

generated = []
for f in genfiles:
    fu = f.replace(".", "_")
    simplerule(
        name=f"em_{fu}",
        ins=[f"./make_{fu}.lua", "h/em_table_lib.lua", "h/em_table"],
        outs=[f"={f}"],
        deps=["h"],
        commands=["$(LUA) $[ins[0]] < $[ins[2]] > $[outs]"],
        label="GENFILE",
    )

clibrary(
    name="em_data",
    srcs=["./em_ptyp.c", ".+em_flag_c", ".+em_mnem_c", ".+em_pseu_c"],
    hdrs={
        "em_spec.h": ".+em_spec_h",
        "em_mnem.h": ".+em_mnem_h",
        "em_pseu.h": ".+em_pseu_h",
    },
    deps=["h"],
)
