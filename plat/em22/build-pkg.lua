include("plat/build.lua")

build_plat_libs {
	name = "libs",
	arch = "em22",
	plat = "em22",
	em = true,
}

installable {
	name = "pkg",
	map = {
		"+libs",
        "./include+pkg",
	}
}

