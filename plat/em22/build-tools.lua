include("plat/build.lua")

return installable {
	name = "tools",
	map = {
		["$(PLATIND)/descr/em22"] = "./descr",
		"util/opt+pkg",
	}
}
