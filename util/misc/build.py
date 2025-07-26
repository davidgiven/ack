from build.ab import export
from build.c import cprogram

cprogram(
    name="em_encode",
    srcs=["./convert.c"],
    deps=[
        "h",
        "modules/h",
        "modules/src/alloc",
        "modules/src/em_code+lib_k",
        "modules/src/read_em+lib_ev",
        "modules/src/string",
        "modules/src/system",
        "modules/src/em_data",
    ],
)

cprogram(
    name="em_decode",
    srcs=["./convert.c"],
    deps=[
        "h",
        "modules/h",
        "modules/src/alloc",
        "modules/src/em_code+lib_e",
        "modules/src/read_em+lib_kv",
        "modules/src/string",
        "modules/src/system",
        "modules/src/em_data",
    ],
)

export(
    name="all",
    items={
        "$(PLATDEP)/em_encode$(EXT)": ".+em_encode",
        "$(PLATDEP)/em_decode$(EXT)": ".+em_decode",
    },
)
