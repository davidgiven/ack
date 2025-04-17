from build.c import clibrary
from glob import glob

clibrary(
    name="system",
    srcs=glob("modules/src/system/*.c"),
    hdrs={"system.h": "./system.h"},
    deps=["modules/src/data", "modules/src/string"],
)
