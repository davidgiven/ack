vars.cflags = {
	"$(CFLAGS)"
}
vars.ackcflags = {
	"-O6"
}
vars.ackldflags = {}
vars.plats = {
	"cpm",
	"linux386",
	"linux68k",
	"linuxppc",
	"linuxmips",
	"osx386",
	"osxppc",
	"pc86",
	"rpi",
	"pdpv7",
	"em22",
}
vars.plats_with_tests = {
	"cpm",
--	"linux386",
--	"linux68k",
--	"linuxmips",
--	"linuxppc",
--	"pc86",
}

local plat_packages = {}
local test_packages = {}
for _, p in ipairs(vars.plats) do
	plat_packages[#plat_packages+1] = "plat/"..p.."+pkg"
end
for _, p in ipairs(vars.plats_with_tests) do
	test_packages[#test_packages+1] = "plat/"..p.."/tests+tests"
end

installable {
	name = "ack",
	map = {
		"lang/basic/src+pkg",
		"lang/cem/cemcom.ansi+pkg",
		"lang/m2/comp+pkg",
		"lang/pc/comp+pkg",
		"lang/b/compiler+pkg",
		"util/ack+pkg",
		"util/amisc+pkg",
		"util/arch+pkg",
		"util/ass+pkg",
		"util/ego+pkg",
		"util/int+pkg",
		"util/led+pkg",
		"util/misc+pkg",
		"util/opt+pkg",
		"examples+pkg",
		plat_packages
	},
}

normalrule {
	name = "tests",
	ins = {
		"first/testsummary.sh",
		test_packages
	},
	outleaves = {
		"stamp"
	},
	commands = {
		"%{ins}"
	}
}
