include("first/bison.lua")

bison {
	name = "bison",
	srcs = { "./mktab.y" }
}

flex {
	name = "flex",
	srcs = { "./scan.l" }
}

local headers = {
	"./alloc.h", "./ext.h", "./getline.h", "./line.h", "./lookup.h", "./optim.h",
	"./param.h", "./pattern.h", "./pop_push.h", "./proinf.h","./putline.h",
	"./reg.h","./tes.h", "./types.h","./util.h"
}

cprogram {
	name = "mktab",
	srcs = {
		matching(filenamesof("+bison"), "%.c$"),
		matching(filenamesof("+flex"), "%.c$"),
	},
	deps = concat(
		headers,
		"+flex",
		"+bison",
		"modules/src/em_data+lib"
	)
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
			"./alloc.c",
			"./backward.c",
			"./cleanup.c",
			"./flow.c",
			"./getline.c",
			"./lookup.c",
			"./main.c",
			"./peephole.c",
			"./process.c",
			"./putline.c",
			"./reg.c",
			"./special.c",
			"./tes.c",
			"./util.c",
			"./var.c",
		},
		deps = concat(
			headers,
			"h+emheaders",
			"modules/src/alloc+lib",
			"modules/src/print+lib",
			"modules/src/string+lib",
			"modules/src/system+lib",
			"modules/src/data+lib",
			"modules/src/em_data+lib"
		),
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

