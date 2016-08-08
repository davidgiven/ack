include("plat/build.lua")
include("lang/build.lua")

build_plat_tools {
	name = "tools",
	arch = "i86",
	plat = "pc86",
}

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "pc86" }
}

acklibrary {
	name = "headers",
	hdrs = {
		["ack/config.h"] = "./include/ack/config.h",
		["unistd.h"] = "./include/unistd.h",
	}
}

build_plat_libs {
	name = "libs",
	arch = "i86",
	plat = "pc86",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		["$(PLATIND)/pc86/boot.o"] = "+boot"
	}
}

