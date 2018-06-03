normalrule {
	name = "bios",
	ins = {
		"util/cmisc+bin2c",
		"./bios",
	},
	outleaves = {"bios.c"},
	commands = {"%{ins[1]} bios < %{ins[2]} > %{outs[1]}"}
}

cprogram {
	name = "8086tiny",
	vars = {
		["+cflags"] = {"-DNO_GRAPHICS", "-fsigned-char"},
	},
	srcs = {
		"./8086tiny.c",
		"+bios"
	}
}

