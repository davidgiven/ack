normalrule {
	name = "tokenfile_g",
	ins = {
		"./make.tokfile",
		"./tokenname.c"
	},
	outleaves = { "tokenfile.g" },
	commands = {
		"sh %{ins[1]} < %{ins[2]} > %{outs}"
	}
}

llgen {
	name = "llgen",
	srcs = {
		-- order here is important
		"+tokenfile_g",
		"./*.g",
	}
}

normalrule {
	name = "parameters_h",
	ins = {
		"./make.parameters",
		"./BigPars",
	},
	outleaves = { "parameters.h" },
	commands = {
		"sh %{ins[1]} < %{ins[2]} > %{outs}"
	}
}

for _, f in ipairs(filenamesof("./*.H")) do
	local name = replace(basename(f), "%.H$", "")
	normalrule {
		name = name.."_h",
		ins = {
			"./make.allocd",
			f
		},
		outleaves = { name..".h" },
		commands = {
			"%{ins[1]} < %{ins[2]} > %{outs}"
		}
	}
end

for _, f in ipairs(filenamesof("./*.C")) do
	local name = replace(basename(f), "%.C$", "")
	normalrule {
		name = name.."_c",
		ins = {
			"./make.allocd",
			f
		},
		outleaves = { name..".c" },
		commands = {
			"%{ins[1]} < %{ins[2]} > %{outs}"
		}
	}
end

normalrule {
	name = "next_c",
	ins = {
		"./make.next",
		"./*.H",
		"./*.C",
	},
	outleaves = { "next.c" },
	commands = {
		"sh %{ins} > %{outs}"
	}
}

normalrule {
	name = "symbol2str_c",
	ins = {
		"./make.tokcase",
		"./tokenname.c",
	},
	outleaves = { "symbol2str.c" },
	commands = {
		"%{ins[1]} < %{ins[2]} > %{outs}"
	}
}

tabgen {
	name = "chartab_c",
	srcs = { "./char.tab" },
}

cprogram {
	name = "em_m2",
	srcs = {
		"./*.c",
		"+casestat_c",
		"+next_c",
		"+scope_c",
		"+symbol2str_c",
		"+tmpvar_c",
		"+chartab_c",
		matching(filenamesof("+llgen"), "%.c$"),
	},
	deps = {
		"+def_h",
		"+llgen",
		"+node_h",
		"+parameters_h",
		"+real_h",
		"+type_h",
		"h+emheaders",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/data+lib",
		"modules/src/em_code+lib_k",
		"modules/src/em_data+lib",
		"modules/src/em_mes+lib",
		"modules/src/flt_arith+lib",
		"modules/src/idf+lib",
		"modules/src/input+lib",
		"modules/src/print+lib",
		"modules/src/string+lib",
		"modules/src/system+lib",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_m2"] = "+em_m2",
		["$(INSDIR)/share/man/man6/em_m2.6"] = "./em_m2.6"
	}
}
