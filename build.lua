vars.cflags = {
	"-g", "-O"
}
vars.ackcflags = {
	"-O"
}
vars.plats = {
	"pc86",
	"cpm",
}

installable {
	name = "ack",
	map = {
		"lang/basic/src+pkg",
		"lang/cem/cemcom.ansi+pkg",
		"lang/m2/comp+pkg",
		"lang/pc/comp+pkg",
		"plat/cpm+pkg",
		"plat/pc86+pkg",
		"util/ack+pkg",
		"util/amisc+pkg",
		"util/arch+pkg",
		"util/misc+pkg",
		"util/opt+pkg",
	}
}

