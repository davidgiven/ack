include("plat/build.lua")

build_as {
	name = "as",
	arch = "ez80",
}

build_ncg {
	name = "ncg",
	arch = "ez80",
}

build_top {
	name = "top",
	arch = "ez80",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/agonmos/as"] = "+as",
		["$(PLATDEP)/agonmos/ncg"] = "+ncg",
		["$(PLATDEP)/agonmos/top"] = "+top",
		["$(PLATIND)/descr/agonmos"] = "./descr",
		"util/opt+pkg",
	}
}
