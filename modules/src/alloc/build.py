from build.c import clibrary

clibrary(
    name="alloc",
    srcs=[
        "./st_alloc.c",
        "./std_alloc.c",
    ],
    hdrs={"alloc.h": "./alloc.h"},
    deps={
        "modules/h",
        "modules/src/system",
    },
)
