include("plat/build.lua")

build_as {
	name = "as",
	arch = "m68020",
}

build_ncg {
	name = "ncg",
	arch = "m68020",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/linux68k/as"] = "+as",
		["$(PLATDEP)/linux68k/ncg"] = "+ncg",
		["$(PLATIND)/descr/linux68k"] = "./descr",
		"util/opt+pkg",
	}
}
