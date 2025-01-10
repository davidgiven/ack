include("plat/build.lua")

build_as {
	name = "as",
	arch = "m68k2",
}

build_ncg {
	name = "ncg",
	arch = "m68020",
	vars = {
		["+cflags"] = "-DWORD_SIZE=2 -DTBL68000=1"
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
