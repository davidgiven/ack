from build.c import clibrary
from glob import glob

clibrary(
    name="object",
    srcs=glob("modules/src/object/*.c") + ["./obj.h"],
    deps=["h"],
    hdrs={"object.h": "./object.h"},
)
