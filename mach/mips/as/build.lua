normalrule {
    name = "astables",
    outleaves = {"definitions.y", "tokens.y", "rules.y"},
    ins = {"./mktables.lua", "./instructions.dat"},
    commands = {"$(LUA) %{ins[1]} %{outs} < %{ins[2]}"}
}

bundle {
	name = "headers",
	srcs = {
		"./mach0.c",
		"./mach1.c",
		"./mach2.c",
		"./mach3.c",
		"./mach4.c",
		"./mach5.c",
	}
}

