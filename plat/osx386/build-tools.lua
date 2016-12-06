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
		["$(PLATDEP)/osx386/as"] = "+as",
		["$(PLATDEP)/osx386/ncg"] = "+ncg",
		["$(PLATIND)/descr/osx386"] = "./descr",
		"plat/osx/cvmach+pkg",
		"util/opt+pkg",
	}
}
