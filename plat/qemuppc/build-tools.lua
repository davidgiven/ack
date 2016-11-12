include("plat/build.lua")

build_as {
	name = "as",
	arch = "powerpc",
}

build_mcg {
	name = "mcg",
	arch = "powerpc",
}

build_ncg {
	name = "ncg",
	arch = "powerpc",
}

build_top {
	name = "top",
	arch = "powerpc",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/qemuppc/as"] = "+as",
		["$(PLATDEP)/qemuppc/ncg"] = "+ncg",
		["$(PLATDEP)/qemuppc/mcg"] = "+mcg",
		["$(PLATDEP)/qemuppc/top"] = "+top",
		["$(PLATIND)/descr/qemuppc"] = "./descr",
		"util/opt+pkg",
	}
}
