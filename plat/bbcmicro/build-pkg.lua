include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "bbcmicro" }
}

build_plat_libs {
	name = "libs",
	arch = "6500",
	plat = "bbcmicro",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/bbcmicro/boot.o"] = "+boot",
        ["$(PLATIND)/bbcmicro/libsys.a"] = "./libsys+lib",
	}
}

