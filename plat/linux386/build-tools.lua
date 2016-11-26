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
		["$(PLATDEP)/linux386/as"] = "+as",
		["$(PLATDEP)/linux386/ncg"] = "+ncg",
		["$(PLATIND)/descr/linux386"] = "./descr",
		"util/amisc+aelflod-pkg",
		"util/opt+pkg",
	}
}
