include("plat/build.lua")

build_as {
	name = "as",
	arch = "z80",
}

build_cg {
	name = "cg",
	arch = "z80",
}

build_top {
	name = "top",
	arch = "z80"
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/spectrum/as"] = "+as",
		["$(PLATDEP)/spectrum/cg"] = "+cg",
		["$(PLATDEP)/spectrum/top"] = "+top",
		["$(PLATIND)/descr/spectrum"] = "./descr",
		"util/opt+pkg",
	}
}
