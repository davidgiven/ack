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
		"./musashi/m68kmmu.h",
		"./m68kconf.h",
		"./musashi/m68kcpu.c",
		"./musashi/m68kfpu.c",
		"./musashi/m68kdasm.c",
		"./musashi/m68k.h",
		"./musashi/softfloat",
	},
	outleaves = {
		"m68kops.c",
		"m68kops.h",
		"m68kcpu.h",
		"m68kconf.h",
		"m68kcpu.c",
		"m68kdasm.c",
		"m68k.h",
	},
	commands = {
		"cp -R %{ins[2]} %{ins[3]} %{ins[4]} %{ins[5]} %{ins[6]} %{ins[7]} %{ins[8]} %{ins[9]} %{ins[10]} %{dir}",
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
		"./musashi/softfloat/softfloat.c",
	},
	deps = {
		"+headers",
	}
}
