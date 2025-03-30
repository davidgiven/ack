from build.c import clibrary

clibrary(
    name="ncg",
    hdrs={
        "mach.h": "./mach.h",
        "mach.c": "./mach.c",
        "instrmacs.h": "./instrmacs.h",
    },
)
