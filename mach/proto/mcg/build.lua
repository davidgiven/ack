normalrule {
	name = "push_pop_c",
	outleaves = { "push_pop.c" },
	ins = {
		"./push_pop.awk",
		"h/em_table"
	},
	commands = {
		"awk -f %{ins[1]} %{ins[2]} > %{outs}"
	}
}

cprogram {
	name = "mcg",
	srcs = {
		"./*.c",
		"+push_pop_c",
	},
	deps = {
		"h+emheaders",
		"modules+headers",
		"modules/src/read_em+lib_kv",
		"modules/src/em_code+lib_k",
		"modules/src/em_data+lib",
		"modules/src/alloc+lib",
		"modules/src/system+lib",
	}
}

-- Just for test purposes for now
installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/mcg"] = "+mcg"
	}
}

