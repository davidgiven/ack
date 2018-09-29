include("plat/build.lua")

build_as {
	name = "as",
	arch = "mips",
	deps = { "mach/mips/as+astables" }
}

build_mcg {
	name = "mcg",
	arch = "mips",
}

build_top {
	name = "top",
	arch = "mips",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/linuxmips/as"] = "+as",
		["$(PLATDEP)/linuxmips/mcg"] = "+mcg",
		["$(PLATDEP)/linuxmips/top"] = "+top",
		["$(PLATIND)/descr/linuxmips"] = "./descr",
		"util/amisc+aelflod-pkg",
		"util/opt+pkg",
	}
}
