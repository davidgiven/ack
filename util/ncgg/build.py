from build.ab import simplerule
from build.c import cprogram
from build.yacc import bison, flex
from glob import glob

bison(name="cggparser", src="./cgg.y")
flex(name="cgglexer", src="./scan.l")

simplerule(
    name="keywords",
    ins=[
        "./make_enterkeyw_c.lua",
        "./keywords",
    ],
    outs=["=enterkeyw.c"],
    commands=["$(LUA) $[ins[0]] < $[ins[1]] > $[outs]"],
)


cprogram(
    name="ncgg",
    srcs=(
        glob("util/ncgg/*.c")
        + glob("util/ncgg/*.h")
        + [".+cggparser", ".+cgglexer", ".+keywords"]
    ),
    deps=["h", "modules/src/em_data"],
)
