vars.cflags = {
	"-g", "-O"
}
vars.ackcflags = {
	"-O"
}
vars.plats = {
	"cpm",
	"linux386",
	"linux68k",
	"pc86",
	"rpi",
}

installable {
	name = "ack",
	map = {
		"lang/basic/src+pkg",
		"lang/cem/cemcom.ansi+pkg",
		"lang/m2/comp+pkg",
		"lang/pc/comp+pkg",
		"plat/cpm+pkg",
		"plat/linux386+pkg",
		"plat/linux68k+pkg",
		"plat/rpi+pkg",
		"plat/pc86+pkg",
		"util/ack+pkg",
		"util/amisc+pkg",
		"util/arch+pkg",
		"util/led+pkg",
		"util/misc+pkg",
		"util/opt+pkg",
	}
}

