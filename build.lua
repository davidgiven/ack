vars.cflags = {
	"-g", "-O"
}
vars.ackcflags = {
	"-O6"
}
vars.plats = {
	"cpm",
	"linux386",
	"linux68k",
	"linuxppc",
	"osx386",
	"osxppc",
	"pc86",
	"rpi",
}

local plat_packages = {}
for _, p in ipairs(vars.plats) do
	plat_packages[#plat_packages+1] = "plat/"..p.."+pkg"
end

installable {
	name = "ack",
	map = {
		"lang/basic/src+pkg",
		"lang/cem/cemcom.ansi+pkg",
		"lang/m2/comp+pkg",
		"lang/pc/comp+pkg",
		"util/ack+pkg",
		"util/amisc+pkg",
		"util/arch+pkg",
		"util/ego+pkg",
		"util/led+pkg",
		"util/misc+pkg",
		"util/opt+pkg",
		"examples+pkg",
		plat_packages
	}
}

