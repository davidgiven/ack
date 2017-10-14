include("plat/build.lua")

build_as {
	name = "as",
	arch = "powerpc",
}

build_mcg {
	name = "mcg",
	arch = "powerpc",
}

build_ncg {
	name = "ncg",
	arch = "powerpc",
}

build_top {
	name = "top",
	arch = "powerpc",
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/linuxppc/as"] = "+as",
		["$(PLATDEP)/linuxppc/ncg"] = "+ncg",
		--["$(PLATDEP)/linuxppc/mcg"] = "+mcg",
		["$(PLATDEP)/linuxppc/top"] = "+top",
		["$(PLATIND)/descr/linuxppc"] = "./descr",
		"util/amisc+aelflod-pkg",
		"util/opt+pkg",
	}
}
