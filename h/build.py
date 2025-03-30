from build.ab import simplerule
from build.c import clibrary

simplerule(
    name="em_path",
    ins=[],
    outs=["=em_path.h"],
    commands=[
        "echo '#define EM_DIR \"$(PREFIX)\"' > $[outs]",
        "echo '#define ACK_PATH \"share/ack/descr\"' >> $[outs]",
    ],
)

simplerule(
    name="local",
    ins=[],
    outs=["=local.h"],
    commands=[
        "echo '#define VERSION 3' > $[outs]",
        "echo '#define ACKM \"$(DEFAULT_PLATFORM)\"' >> $[outs]",
        "echo '#define BIGMACHINE 1' >> $[outs]",
        "echo '#define SYS_5' >> $[outs]",
    ],
)

clibrary(
    name="h",
    hdrs={
        k: f"./{k}"
        for k in [
            "arch.h",
            "as_spec.h",
            "ip_spec.h",
            "cg_pattern.h",
            "cgg_cg.h",
            "con_float",
            "em_abs.h",
            "em_ego.h",
            "em_flag.h",
            "em_mes.h",
            "em_ptyp.h",
            "em_reg.h",
            "out.h",
            "ranlib.h",
            "stb.h",
        ]
    }
    | {"local.h": ".+local", "em_path.h": ".+em_path"},
)
