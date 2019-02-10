include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "linuxmips" }
}

build_plat_libs {
	name = "libs",
	arch = "mips",
	plat = "linuxmips",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		"./include+pkg",
		["$(PLATIND)/linuxmips/boot.o"] = "+boot",
        ["$(PLATIND)/linuxmips/libsys.a"] = "./libsys+lib",
	}
}

