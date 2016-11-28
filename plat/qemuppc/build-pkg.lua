include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "qemuppc" }
}

build_plat_libs {
	name = "libs",
	arch = "powerpc",
	plat = "qemuppc",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		"./include+pkg",
		"util/amisc+aslod-pkg",
		["$(PLATIND)/qemuppc/boot.o"] = "+boot",
        ["$(PLATIND)/qemuppc/libsys.a"] = "./libsys+lib",
	}
}

