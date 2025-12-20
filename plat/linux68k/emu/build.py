from build.ab import simplerule
from build.c import hostcprogram, hostclibrary
from build.ack import ackcfile
from build.utils import objectify

hostcprogram(name="m68kmake", srcs=["./musashi/m68kmake.c"])

simplerule(
    name="m68k_engine",
    ins=[
        ".+m68kmake",
        "./musashi/m68k_in.c",
        "./musashi/m68kcpu.h",
        "./musashi/m68kmmu.h",
        "./m68kconf.h",
        "./musashi/m68kcpu.c",
        "./musashi/m68kfpu.c",
        "./musashi/m68kdasm.c",
        "./musashi/m68k.h",
        # "./musashi/softfloat/mamesf.h",
        # "./musashi/softfloat/milieu.h",
        # "./musashi/softfloat/softfloat.c",
        # "./musashi/softfloat/softfloat.h",
    ],
    outs=[
        "=m68kops.c",
        "=m68kops.h",
        "=m68kcpu.h",
        "=m68kconf.h",
        "=m68kcpu.c",
        "=m68kdasm.c",
        "=m68k.h",
    ],
    commands=["cp $[ins] $[dir]", "cd $[dir] && ./m68kmake"],
)

hostclibrary(
    name="support",
    hdrs={
        "softfloat/milieu.h": "./musashi/softfloat/milieu.h",
        "softfloat/mamesf.h": "./musashi/softfloat/mamesf.h",
        "softfloat/softfloat.h": "./musashi/softfloat/softfloat.h",
        "m68kfpu.c": "./musashi/m68kfpu.c",
        "m68kmmu.h": "./musashi/m68kmmu.h",
    },
)

hostcprogram(
    name="emu",
    srcs=[
        ".+m68k_engine",
        "./sim.c",
        "./musashi/softfloat/softfloat.c",
        "./musashi/softfloat/softfloat-macros",
        "./musashi/softfloat/softfloat-specialize",
        "./sim.h",
        "./m68kconf.h",
    ],
    deps=[
        ".+support",
    ],
    cflags=["-DM68K_COMPILE_FOR_MAME=0"],
)
