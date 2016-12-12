include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "osxppc" }
}

build_plat_libs {
	name = "libs",
	arch = "powerpc",
	plat = "osxppc",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		"plat/osx/include+pkg",
		["$(PLATIND)/osxppc/boot.o"] = "+boot",
		["$(PLATIND)/osxppc/libsys.a"] = "./libsys+lib",
	}
}
