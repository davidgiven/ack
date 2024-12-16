include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "spectrum" }
}

build_plat_libs {
	name = "libs",
	arch = "z80",
	plat = "spectrum",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/spectrum/boot.o"] = "+boot",
        ["$(PLATIND)/spectrum/libsys.a"] = "./libsys+lib",
	}
}

