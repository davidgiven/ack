include("plat/build.lua")

--build_as {
--	name = "as",
--	arch = "i80",
--}

build_ncg {
	name = "ncg",
	arch = "ez80",
}

--build_top {
--	name = "top",
--	arch = "i80",
--}

return installable {
	name = "tools",
	map = {
--		["$(PLATDEP)/cpm/as"] = "+as",
		["$(PLATDEP)/agonmos/ncg"] = "+ncg",
--		["$(PLATDEP)/cpm/top"] = "+top",
--		["$(PLATIND)/descr/cpm"] = "./descr",
		"util/opt+pkg",
	}
}
