ackfile {
	name = "bdos_o",
	srcs = {
		"./bdos.s",
	},
	vars = { plat = "cpm" },
}

normalrule {
	name = "bdos_out",
	ins = {
		"util/led+led",
		"+bdos_o",
	},
	outleaves = { "bdos.out" },
	commands = { "%{ins[1]} -b0:0xff00 %{ins[2]} -o %{outs[1]}" }
}

normalrule {
	name = "bdos_img",
	ins = {
		"util/amisc+aslod",
		"+bdos_out",
	},
	outleaves = { "bdos.img" },
	commands = { "%{ins[1]} %{ins[2]} %{outs[1]}" }
}

normalrule {
	name = "bdos_c",
	ins = {
		"util/cmisc+objectify",
		"+bdos_img",
	},
	outleaves = { "bdos.c" },
	commands = { "%{ins[1]} bdos < %{ins[2]} > %{outs[1]}" }
}

cprogram {
	name = "emu",
	srcs = {
		"+bdos_c",
		"./biosbdos.c",
		"./dis8080.c",
		"./emulator.c",
		"./fileio.c",
		"./intel_8080_emulator.c",
		"./main.c",
	},
}

