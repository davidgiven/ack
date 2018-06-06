cprogram {
	name = "m68kmake",
	srcs = {
		"./musashi/m68kmake.c"
	}
}

normalrule {
	name = "m68k_engine",
	ins = {
		"+m68kmake",
		"./musashi/m68k_in.c",
		"./musashi/m68kcpu.h",
		"./m68kconf.h",
		"./musashi/m68kcpu.c",
		"./musashi/m68kdasm.c",
		"./musashi/m68k.h",
	},
	outleaves = {
		"m68kopac.c",
		"m68kopdm.c",
		"m68kopnz.c",
		"m68kops.c",
		"m68kops.h",
		"m68kcpu.h",
		"m68kconf.h",
		"m68kcpu.c",
		"m68kdasm.c",
		"m68k.h",
	},
	commands = {
		"cp %{ins[2]} %{ins[3]} %{ins[4]} %{ins[5]} %{ins[6]} %{ins[7]} %{dir}",
		"cd %{dir} && %{ins[1]}"
	}
}

clibrary {
	name = "headers",
	srcs = {},
	hdrs = {
		matching(filenamesof("+m68k_engine"), "%.h$"),
	}
}

cprogram {
	name = "emu68k",
	vars = {
		["+cflags"] = {"-DM68K_COMPILE_FOR_MAME=0"}
	},
	srcs = {
		"./sim.c",
		matching(filenamesof("+m68k_engine"), "%.c$"),
	},
	deps = {
		"+headers",
	}
}

