include("first/yacc.lua")

yacc {
	name = "yacc",
	srcs = { "./mktab.y" }
}

flex {
	name = "flex",
	srcs = { "./scan.l" }
}

cprogram {
	name = "mktab",
	srcs = {
		matching(filenamesof("+yacc"), "%.c$"),
		matching(filenamesof("+flex"), "%.c$"),
	},
	deps = {
		"+flex",
		"+yacc",
		"util/data+em_data",
	}
}

normalrule {
	name = "pattern_c",
	ins = {
		"+mktab",
		"./patterns",
		"lang/cem/cpp.ansi+cpp"
	},
	outleaves = { "pattern.c" },
	commands = {
		"%{ins[3]} < %{ins[2]} | %{ins[1]} > %{outs}"
	}
}

normalrule {
	name = "pop_push_c",
	ins = {
		"./pop_push.awk",
		"h/em_table"
	},
	outleaves = { "pop_push.c" },
	commands = {
		"awk -f %{ins[1]} < %{ins[2]} > %{outs}"
	}
}

local function variant(name, cflags)
	cprogram {
		name = name,
		srcs = {
			"+pattern_c",
			"+pop_push_c",
			"./*.c",
		},
		deps = {
			"./*.h",
			"h+emheaders",
			"modules/src/alloc+lib",
			"modules/src/print+lib",
			"modules/src/string+lib",
			"modules/src/system+lib",
			"util/data+em_data",
		},
		vars = {
			["+cflags"] = cflags
		}
	}
end

variant("em_opt", {})
variant("em_opt2", {"-DGLOBAL_OPT"})

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_opt"] = "+em_opt",
		["$(PLATDEP)/em_opt2"] = "+em_opt2",
		["$(INSDIR)/share/man/man6/em_opt.6"] = "./em_opt.6",
	}
}

