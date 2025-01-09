include("plat/build.lua")

build_as {
	name = "as",
	arch = "m68k2",
}

build_ncg {
	name = "ncg",
	arch = "m68k2",
	deps = {
		"mach/m68020/ncg/table"
	}
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/minix68k/as"] = "+as",
		["$(PLATDEP)/minix68k/ncg"] = "+ncg",
		["$(PLATIND)/descr/minix68k"] = "./descr",
		"util/opt+pkg",
	}
}
