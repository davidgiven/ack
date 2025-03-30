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
            "out.h",
            "arch.h",
            "ranlib.h",
            "em_abs.h",
            "em_ego.h",
            "em_flag.h",
            "em_ptyp.h",
            "em_mes.h",
            "cgg_cg.h",
            "em_reg.h",
            "stb.h",
            "con_float",
        ]
    }
    | {"local.h": ".+local", "em_path.h": ".+em_path"},
)
