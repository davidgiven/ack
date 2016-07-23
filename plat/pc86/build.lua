include("mach/proto/as/build.lua")

build_as {
	name = "as",
	arch = "i86"
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/pc86/as"] = "+as"
	}
}

