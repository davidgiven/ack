include("plat/build.lua")

build_as {
	name = "as",
	arch = "pdp",
}

build_cg {
	name = "cg",
	arch = "pdp",
}

build_top {
	name = "top",
	arch = "pdp"
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/pdpv7/as"] = "+as",
		["$(PLATDEP)/pdpv7/cg"] = "+cg",
		["$(PLATDEP)/pdpv7/cv"] = "mach/pdp/cv+cv",
		["$(PLATDEP)/pdpv7/top"] = "+top",
		["$(PLATIND)/descr/pdpv7"] = "./descr",
		"util/opt+pkg",
	}
}
