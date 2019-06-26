include("plat/build.lua")

--build_as {
--	name = "as",
--	arch = "z80",
--}

build_mcg {
	name = "mcg",
	arch = "z80",
	vars = {
	}
}

--build_top {
--	name = "top",
--	arch = "i80",
--}

return installable {
	name = "tools",
	map = {
--		["$(PLATDEP)/cpm/as"] = "+as",
		["$(PLATDEP)/cpmz/mcg"] = "+mcg",
--		["$(PLATDEP)/cpm/top"] = "+top",
--		["$(PLATIND)/descr/cpm"] = "./descr",
		"util/opt+pkg",
	}
}
