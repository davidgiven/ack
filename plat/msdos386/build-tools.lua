include("plat/build.lua")

build_as {
	name = "as",
	arch = "i386",
}

build_ncg {
	name = "ncg",
	arch = "i386",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/msdos386/as"] = "+as",
		["$(PLATDEP)/msdos386/ncg"] = "+ncg",
		["$(PLATIND)/descr/msdos386"] = "./descr",
		"util/opt+pkg",
	}
}
