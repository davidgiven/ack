from build.c import clibrary

clibrary(
    name="data",
    srcs=[
        "./array.c",
        "./astring.c",
        "./warnings.c",
        "./imap.c",
        "./pmap.c",
        "./smap.c",
        "./stringlist.c",
    ],
    hdrs={
        k: f"./{k}"
        for k in [
            "./array.h",
            "./astring.h",
            "./warnings.h",
            "./imap.h",
            "./pmap.h",
            "./smap.h",
            "./stringlist.h",
        ]
    },
)
