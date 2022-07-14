include("util/LLgen/build.lua")

llgen {
	name = "llgen",
	srcs = { "./basic.g" }
}

normalrule {
	name = "tokentab_h",
	ins = {
		"./maketokentab",
		"util/cmisc+ed",
		matching(filenamesof("+llgen"), "/Lpars.h$"),
	},
	outleaves = { "tokentab.h" },
	commands = {
		"%{ins} %{outs}"
	}
}

cprogram {
	name = "em_bem",
	srcs = {
		"./bem.c",
		"./compile.c",
		"./eval.c",
		"./func.c",
		"./gencode.c",
		"./graph.c",
		"./initialize.c",
		"./parsepar.c",
		"./symbols.c",
		"./util.c",
		matching(filenamesof("+llgen"), "%.c$"),
	},
	deps = {
		"./bem.h",
		"./eval.h",
		"./func.h",
		"./gencode.h",
		"./graph.h",
		"./llmess.h",
		"./parsepar.h",
		"./symbols.h",
		"./util.h",
		"./yylexp.h",
		"+llgen",
		"+tokentab_h",
		"h+emheaders",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/em_code+lib_k",
		"modules/src/em_data+lib",
		"modules/src/em_mes+lib",
		"modules/src/print+lib",
		"modules/src/string+lib",
		"modules/src/system+lib",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_bem"] = "+em_bem"
	}
}

