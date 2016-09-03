include("plat/build.lua")

build_as {
	name = "as",
	arch = "i86",
}

build_ncg {
	name = "ncg",
	arch = "i86",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/pc86/as"] = "+as",
		["$(PLATDEP)/pc86/ncg"] = "+ncg",
		["$(PLATIND)/descr/pc86"] = "./descr",
		"util/opt+pkg",
	}
}
