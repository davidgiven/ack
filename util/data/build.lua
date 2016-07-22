normalrule {
	name = "generated",
	ins = {
		"./new_table",
		"h/em_table", -- relative to root, which is a bit evil
	},
	outleaves = {
		"em_flag.c",
		"em_pseu.c",
		"em_mnem.c",
		"em_spec.h",
		"em_pseu.h",
		"em_mnem.h",
	},
	deps = {
		"h+emheaders"
	},
	commands = {
		"%{ins[1]} %{ins[2]} %{dir} %{dir}"
	}
}

clibrary {
	name = "em_data",
	srcs = {
		"./em_ptyp.c",
		"+generated", -- so we build the C files
	},
	hdrs = {
		"+generated" -- so we export the H files
	},
	deps = {
		"+generated", -- so we can see the H files
		"h+emheaders"
	}
}
