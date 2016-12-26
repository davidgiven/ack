include("plat/build.lua")

build_as {
	name = "as",
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
		["$(PLATDEP)/osxppc/as"] = "+as",
		["$(PLATDEP)/osxppc/ncg"] = "+ncg",
		["$(PLATDEP)/osxppc/top"] = "+top",
		["$(PLATIND)/descr/osxppc"] = "./descr",
		"plat/osx/cvmach+pkg",
		"util/opt+pkg",
	}
}
