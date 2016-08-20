include("plat/build.lua")

build_as {
	name = "as",
	arch = "vc4",
}

build_ncg {
	name = "ncg",
	arch = "vc4",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/rpi/as"] = "+as",
		["$(PLATDEP)/rpi/ncg"] = "+ncg",
		["$(PLATIND)/descr/rpi"] = "./descr",
		"util/opt+pkg",
	}
}
