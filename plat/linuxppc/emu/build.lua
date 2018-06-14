normalrule {
	name = "dispatcher",
	ins = {
		"./mkdispatcher.lua",
		"./instructions.dat"
	},
	outleaves = {
		"dispatcher.h"
	},
	commands = {
		"$(LUA) %{ins[1]} < %{ins[2]} > %{outs}"
	}
}

clibrary {
	name = "dispatcher_lib",
	srcs = {},
	hdrs = { "+dispatcher" }
}

cprogram {
	name = "emuppc",
	srcs = { "./*.c" },
	deps = {
		"+dispatcher_lib"
	}
}

