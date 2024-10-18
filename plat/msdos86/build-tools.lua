include("plat/build.lua")

build_as {
	name = "as",
	arch = "i86",
}

build_ncg {
	name = "ncg",
	arch = "i86",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/msdos86/as"] = "+as",
		["$(PLATDEP)/msdos86/ncg"] = "+ncg",
		["$(PLATIND)/descr/msdos86"] = "./descr",
		"util/opt+pkg",
		"util/amisc+aslod-pkg",
	}
}
