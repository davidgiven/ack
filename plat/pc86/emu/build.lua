clibrary {
	name = "x86emu",
	vars = {
		["+cflags"] = {"-Iplat/pc86/emu/x86emu", "-DDEBUG"}
	},
	srcs = {
		"./x86emu/debug.c",
		"./x86emu/decode.c",
		"./x86emu/fpu.c",
		"./x86emu/ops2.c",
		"./x86emu/ops.c",
		"./x86emu/prim_ops.c",
		"./x86emu/sys.c",
	}	
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

