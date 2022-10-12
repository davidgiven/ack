include("plat/build.lua")

build_as {
	name = "as",
	arch = "6500",
}

build_cg {
	name = "cg",
	arch = "6500",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/bbcmicro/as"] = "+as",
		["$(PLATDEP)/bbcmicro/cg"] = "+cg",
		["$(PLATIND)/descr/bbcmicro"] = "./descr",
		"util/opt+pkg",
	}
}
