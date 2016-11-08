include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "osx386" }
}

build_plat_libs {
	name = "libs",
	arch = "i386",
	plat = "osx386",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		"plat/osx/include+pkg",
		["$(PLATIND)/osx386/boot.o"] = "+boot",
		["$(PLATIND)/osx386/libsys.a"] = "./libsys+lib",
	}
}
