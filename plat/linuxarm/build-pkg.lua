include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "linuxarm" }
}

build_plat_libs {
	name = "libs",
	arch = "arm",
	plat = "linuxarm",
	libraries = {
		libend = true
	}
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		"./include+pkg",
		["$(PLATIND)/linuxarm/boot.o"] = "+boot",
		--["$(PLATIND)/linuxarm/libsys.a"] = "./libsys+lib",
	}
}
