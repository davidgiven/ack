include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "rpi" }
}

build_plat_libs {
	name = "libs",
	arch = "vc4",
	plat = "rpi",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/rpi/boot.o"] = "+boot",
        ["$(PLATIND)/rpi/libsys.a"] = "./libsys+lib",
	}
}

