include("plat/build.lua")

build_as {
	name = "as",
	arch = "mips",
	deps = { "mach/mips/as+astables" }
}

--build_mcg {
--	name = "mcg",
--	arch = "powerpc",
--}
--
--build_top {
--	name = "top",
--	arch = "powerpc",
--}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/linuxmips/as"] = "+as",
		--["$(PLATDEP)/linuxppc/ncg"] = "+ncg",
		--["$(PLATDEP)/linuxppc/mcg"] = "+mcg",
		--["$(PLATDEP)/linuxppc/top"] = "+top",
		["$(PLATIND)/descr/linuxmips"] = "./descr",
		"util/amisc+aelflod-pkg",
		"util/opt+pkg",
	}
}
