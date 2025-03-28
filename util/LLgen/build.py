from build.ab import Rule, Targets, simplerule, filenamesof
from build.c import hostcprogram
from os.path import *

hostcprogram(
    name="LLgen",
    # These use pre-LLgen'd versions of LLgen.c, Lpars.c, Lpars.h, and
    # tokens.c. If LLgen.g or tokens.g gets updated, they need
    # rebuilding. Use the bootstrap target to do this.
    srcs=[
        "./src/LLgen.c",
        "./src/Lpars.c",
        "./src/alloc.c",
        "./src/cclass.c",
        "./src/check.c",
        "./src/compute.c",
        "./src/gencode.c",
        "./src/global.c",
        "./src/machdep.c",
        "./src/main.c",
        "./src/name.c",
        "./src/reach.c",
        "./src/savegram.c",
        "./src/sets.c",
        "./src/tokens.c",
        "./src/utils.c",
        "./src/cclass.h",
        "./src/extern.h",
        "./src/fileio.h",
        "./src/Lpars.h",
        "./src/sets.h",
        "./src/types.h",
    ],
    cflags=['-DLIBDIR=\\"util/LLgen/lib\\"', "-DNON_CORRECTING"],
)


@Rule
def llgen(self, name, srcs: Targets = []):
    fs = [splitext(basename(f))[0] for f in filenamesof(srcs)]
    simplerule(
        replaces=self,
        ins=["util/LLgen"] + srcs,
        outs=(["=Lpars.c", "=Lpars.h"] + [f"={f}.c" for f in fs]),
        deps=["util/LLgen/lib/incl", "util/LLgen/lib/rec"],
        commands=[
            f"$[ins[0]] {' '.join(filenamesof(srcs))}",
            # llgen emits the files into the current directory (of the sandbox).
            f"mv *.[ch] $[dir]",
        ],
        label="LLGEN",
    )
