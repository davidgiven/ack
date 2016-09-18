normalrule {
	name = "ircodes",
	outleaves = { "ircodes.h", "ircodes.c" },
	ins = {
		"./ircodes.sh",
		"./ir.dat"
	},
	commands = {
		"%{ins[1]} %{ins[2]} %{outs[1]} %{outs[2]}"
	}
}

cprogram {
	name = "mcg",
	srcs = {
		"./*.c",
		matching(filenamesof("+ircodes"), "%.c$")
	},
	deps = {
		"+ircodes",
		"h+emheaders",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/em_code+lib_k",
		"modules/src/em_data+lib",
		"modules/src/idf+lib",
		"modules/src/read_em+lib_kv",
		"modules/src/system+lib",
		"./*.h",
	},
	vars = {
		["+cflags"] = {"-Werror-implicit-function-declaration"}
	}
}

-- Just for test purposes for now
installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/mcg"] = "+mcg"
	}
}

