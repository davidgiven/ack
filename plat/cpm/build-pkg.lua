include("plat/build.lua")
include("lang/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "cpm" }
}

build_plat_libs {
	name = "libs",
	arch = "i80",
	plat = "cpm",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/cpm/boot.o"] = "+boot"
	}
}

