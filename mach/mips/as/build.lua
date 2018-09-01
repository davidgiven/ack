normalrule {
    name = "astables",
    outleaves = {"definitions.y", "tokens.y", "rules.y"},
    ins = {"./mktables.lua", "./instructions.dat"},
    commands = {"$(LUA) %{ins[1]} %{outs} < %{ins[2]}"}
}
