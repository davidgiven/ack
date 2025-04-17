from build.ab import simplerule
from build.c import hostcprogram

simplerule(
    name="dispatcher",
    ins=["./mkdispatcher.lua", "./instructions.dat"],
    outs=["=dispatcher.h"],
    commands=["$(LUA) $[ins[0]] < $[ins[1]] > $[outs]"],
)

hostcprogram(
    name="emu", srcs=[".+dispatcher", "./emu.c", "./emu.h", "./main.c"]
)
