include("plat/build.lua")
include("lang/build.lua")

--ackfile {
--	name = "boot",
--	srcs = { "./boot.s" },
--	vars = {
--		plat = "cpm",
--		["+ackcflags"] = "-DUSE_I80_RSTS",
--	}
--}

--build_plat_libs {
--	name = "libs",
--	arch = "i80",
--	plat = "cpm",
--}

installable {
	name = "pkg",
	map = {
		"+tools",
--		"+libs",
 --       "./include+pkg",
--		["$(PLATIND)/cpm/boot.o"] = "+boot",
--        ["$(PLATIND)/cpm/libsys.a"] = "./libsys+lib",
	}
}

