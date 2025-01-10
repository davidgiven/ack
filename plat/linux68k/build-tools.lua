include("plat/build.lua")

build_as {
	name = "as",
	arch = "m68020",
}

build_ncg {
	name = "ncg",
	arch = "m68020",
	vars = {
		["+cflags"] = "-DWORD_SIZE=4 -DTBL68020=1 -DTBL68881=1"
	}
}

return installable {
	name = "tools",
	map = {
		["$(PLATDEP)/linux68k/as"] = "+as",
		["$(PLATDEP)/linux68k/ncg"] = "+ncg",
		["$(PLATIND)/descr/linux68k"] = "./descr",
		"util/amisc+aelflod-pkg",
		"util/opt+pkg",
	}
}
