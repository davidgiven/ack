include("plat/build.lua")
include("lang/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = {
		plat = "agonmos",
	}
}

build_plat_libs {
	name = "libs",
	arch = "ez80",
	plat = "agonmos",
	exclude = { b=true, pc=true, m2=true },
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/agonmos/boot.o"] = "+boot",
        ["$(PLATIND)/agonmos/libsys.a"] = "./libsys+lib",
	}
}

