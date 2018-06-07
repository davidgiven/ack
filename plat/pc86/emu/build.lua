clibrary {
	name = "x86emu",
	vars = {
		["+cflags"] = {"-Iplat/pc86/emu/x86emu", "-DDEBUG"}
	},
	srcs = {"./x86emu/*.c"}
}

cprogram {
	name = "pc86emu",
	srcs = {"./main.c"},
	vars = {
		["+cflags"] = {"-Iplat/pc86/emu/x86emu", "-DDEBUG"}
	},
	deps = {
		"+x86emu"
	}
}

