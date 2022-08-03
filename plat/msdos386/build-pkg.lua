include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "msdos386" }
}

build_plat_libs {
	name = "libs",
	arch = "i386",
	plat = "msdos386",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/msdos386/boot.o"] = "+boot",
        ["$(PLATIND)/msdos386/libsys.a"] = "./libsys+lib",
	}
}

