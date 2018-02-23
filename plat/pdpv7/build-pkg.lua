include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "pdpv7" }
}

build_plat_libs {
	name = "libs",
	arch = "pdp",
	plat = "pdpv7",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/pdpv7/boot.o"] = "+boot",
        ["$(PLATIND)/pdpv7/libsys.a"] = "./libsys+lib",
	}
}

