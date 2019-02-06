include("plat/build.lua")

build_as {
	name = "as",
	arch = "i80",
}

build_ncg {
	name = "ncg",
	arch = "i80",
	vars = {
		["+cflags"] = "-DUSE_I80_RSTS"
	}
}

build_top {
	name = "top",
	arch = "i80",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/cpm/as"] = "+as",
		["$(PLATDEP)/cpm/ncg"] = "+ncg",
		["$(PLATDEP)/cpm/top"] = "+top",
		["$(PLATIND)/descr/cpm"] = "./descr",
		"util/opt+pkg",
	}
}
