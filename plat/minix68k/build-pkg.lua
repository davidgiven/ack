include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "minix68k" }
}

build_plat_libs {
	name = "libs",
	arch = "m68k2",
	plat = "minix68k",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		"./include+pkg",
		["$(PLATIND)/minix68k/boot.o"] = "+boot",
        ["$(PLATIND)/minix68k/libsys.a"] = "./libsys+lib",
	}
}

