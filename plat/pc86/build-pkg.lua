include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "pc86" }
}

build_plat_libs {
	name = "libs",
	arch = "i86",
	plat = "pc86",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/pc86/boot.o"] = "+boot",
        ["$(PLATIND)/pc86/libsys.a"] = "./libsys+lib",
	}
}

