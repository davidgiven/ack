include("plat/build.lua")

ackfile {
	name = "boot",
	srcs = { "./boot.s" },
	vars = { plat = "msdos386" }
}

ackfile {
	name = "stub",
	srcs = { "./stub.s" },
	deps = { "plat/msdos386/libsys+headers" },
	vars = { plat = "msdos386" }
}

normalrule {
	name = "stub_aout",
	ins = {
		"util/led+led",
		"+stub"
	},
	outleaves = { "stub.aout" },
	commands = { "%{ins[1]} %{ins[2]} -o %{outs[1]}" }
}

normalrule {
	name = "stub_exe",
	ins = {
		"util/amisc+aslod",
		"+stub_aout"
	},
	outleaves = { "stub.exe" },
	commands = { "%{ins[1]} %{ins[2]} %{outs[1]}" }
}

build_plat_libs {
	name = "libs",
	arch = "i386",
	plat = "msdos386",
}

installable {
	name = "pkg",
	map = {
		"+tools",
		"+libs",
        "./include+pkg",
		["$(PLATIND)/msdos386/boot.o"] = "+boot",
		["$(PLATIND)/msdos386/stub.exe"] = "+stub_exe",
        ["$(PLATIND)/msdos386/libsys.a"] = "./libsys+lib",
	}
}

