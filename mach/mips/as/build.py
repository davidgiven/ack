from build.ab import simplerule, filenamesof, targetof
from build.c import clibrary

simplerule(
    name="astables",
    ins=["./mktables.lua", "./instructions.dat"],
    outs=["=definitions.y", "=tokens.y", "=rules.y"],
    commands=["$(LUA) $[ins[0]] $[outs] < $[ins[1]"],
)

h = filenamesof([targetof(".+astables")])
clibrary(
    name="lib", hdrs={"definitions.y": h[0], "tokens.y": h[1], "rules.y": h[2]}
)
