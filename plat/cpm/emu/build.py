from build.ab import simplerule
from build.c import hostcprogram
from build.ack import ackcfile
from build.utils import objectify

ackcfile(name="bdos_o", srcs=["./bdos.s"], plat="cpm")

simplerule(
    name="bdos_out",
    ins=[
        "util/led",
        ".+bdos_o",
    ],
    outs=["=bdos.out"],
    commands=["$[ins[0]] -b0:0xff00 $[ins[1]] -o $[outs]"],
)

simplerule(
    name="bdos_img",
    ins=[
        "util/amisc+aslod",
        ".+bdos_out",
    ],
    outs=["bdos.img"],
    commands=["$[ins] $[outs]"],
)

objectify(name="bdos_c", src=".+bdos_img", symbol="bdos_data")

hostcprogram(
    name="emu",
    srcs=[
        ".+bdos_c",
        "./biosbdos.c",
        "./dis8080.c",
        "./dis8080.h",
        "./emulator.c",
        "./fileio.c",
        "./intel_8080_emulator.c",
        "./intel_8080_emulator.h",
        "./main.c",
        "./globals.h",
    ],
)
