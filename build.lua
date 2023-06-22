vars.cflags = {
	"$(CFLAGS)"
}
vars.ldflags = {}
vars.ackcflags = {
	"-O6"
}
vars.ackldflags = {}
vars.plats = {
	"agonmos",
	"cpm",
	--"linux386",
	--"linux68k",
	--"linuxppc",
	--"linuxmips",
	--"msdos86",
	--"msdos386",
	--"osx386",
	--"osxppc",
	--"pc86",
	--"rpi",
	--"pdpv7",
	--"em22",
}
vars.plats_with_tests = {
	"agonmos",
	"cpm",
	--"linux68k",
	--"linuxppc",
	--"pc86",
}

local is_windows = os.getenv("OS") == "Windows_NT"

local int = {}
if not is_windows then
	int[#int+1] = "util/int+pkg"
end

installable {
	name = "ack-common",
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
		"util/led+pkg",
		"util/misc+pkg",
		"util/opt+pkg",
		int
	},
}

local plat_packages = {}
local test_packages = {}
for _, p in ipairs(vars.plats) do
	local pkg = "plat/"..p.."+pkg"
	plat_packages[#plat_packages+1] = pkg

	installable {
		name = "ack-"..p,
		map = {
			"+ack-common",
			pkg,
		},
	}
end
for _, p in ipairs(vars.plats_with_tests) do
	test_packages[#test_packages+1] = "plat/"..p.."/tests+tests"
end

installable {
	name = "ack",
	map = {
		"+ack-common",
		"examples+pkg",
		plat_packages
	},
}

if not is_windows then
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
else
	normalrule {
		name = "tests",
		ins = {},
		outleaves = {
			"stamp"
		},
		commands = {
			"touch %{outs}"
		}
	}
end
