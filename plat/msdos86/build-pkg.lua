include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = {
		["+ackcflags"] = { "-DSEPID=0" },
		plat = "msdos86"
	}
}

ackfile {
	name = "boot-sepid",
	srcs = { "./boot.s" },
	vars = {
		["+ackcflags"] = { "-DSEPID=1" },
		plat = "msdos86"
	}
}

build_plat_libs {
	name = "libs",
	arch = "i86",
	plat = "msdos86",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
		"./include+pkg",
		["$(PLATIND)/msdos86/boot.o"] = "+boot",
		["$(PLATIND)/msdos86/boot-sepid.o"] = "+boot-sepid",
		["$(PLATIND)/msdos86/libsys.a"] = "./libsys+lib",
	}
}

