include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "linux68k" }
}

build_plat_libs {
	name = "libs",
	arch = "m68020",
	plat = "linux68k",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		"./include+pkg",
		["$(PLATIND)/linux68k/boot.o"] = "+boot",
        ["$(PLATIND)/linux68k/libsys.a"] = "./libsys+lib",
	}
}

