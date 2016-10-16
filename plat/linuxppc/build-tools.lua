include("plat/build.lua")

build_as {
	name = "as",
	arch = "powerpc",
}

build_mcg {
	name = "mcg",
	arch = "powerpc",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/linuxppc/as"] = "+as",
		["$(PLATDEP)/linuxppc/mcg"] = "+mcg",
		["$(PLATIND)/descr/linuxppc"] = "./descr",
		"util/opt+pkg",
	}
}
