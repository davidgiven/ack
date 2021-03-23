include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "msdos86" }
}

build_plat_libs {
	name = "libs",
	arch = "i86",
	plat = "msdos86",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/msdos86/boot.o"] = "+boot",
        ["$(PLATIND)/msdos86/libsys.a"] = "./libsys+lib",
	}
}

