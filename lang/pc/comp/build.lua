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
		"./declar.g",
		"./expression.g",
		"./program.g",
		"./statement.g",
	}
}

normalrule {
	name = "parameters_h",
	ins = {
		"./make.parameters",
		"./Parameters",
	},
	outleaves = { "parameters.h" },
	commands = {
		"sh %{ins[1]} < %{ins[2]} > %{outs}"
	}
}

local xh_files = {
	"./def.xh",
	"./desig.xh",
	"./node.xh",
	"./scope.xh",
	"./type.xh",
}

for _, f in ipairs(filenamesof(xh_files)) do
	local name = replace(basename(f), "%.xh$", "")
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

local xc_files = {
	"./casestat.xc",
	"./tmpvar.xc",
}

for _, f in ipairs(filenamesof(xc_files)) do
	local name = replace(basename(f), "%.xc$", "")
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
		xh_files,
		xc_files
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
	name = "em_pc",
	srcs = {
		"./body.c",
		"./chk_expr.c",
		"./code.c",
		"./cstoper.c",
		"./def.c",
		"./desig.c",
		"./enter.c",
		"./error.c",
		"./idf.c",
		"./input.c",
		"./label.c",
		"./LLlex.c",
		"./LLmessage.c",
		"./lookup.c",
		"./main.c",
		"./misc.c",
		"./node.c",
		"./options.c",
		"./progs.c",
		"./readwrite.c",
		"./scope.c",
		"./stab.c",
		"./tokenname.c",
		"./type.c",
		"./typequiv.c",
		"+casestat_c",
		"+chartab_c",
		"+next_c",
		"+symbol2str_c",
		"+tmpvar_c",
		matching(filenamesof("+llgen"), "%.c$"),
	},
	deps = {
		"+def_h",
		"+desig_h",
		"+llgen",
		"+node_h",
		"+parameters_h",
		"+scope_h",
		"+type_h",
		"h+emheaders",
		"lang/pc/include+headers",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/em_code+lib_k",
		"modules/src/em_data+lib",
		"modules/src/em_mes+lib",
		"modules/src/flt_arith+lib",
		"modules/src/idf+lib",
		"modules/src/input+lib",
		"modules/src/print+lib",
		"modules/src/string+lib",
		"modules/src/system+lib",
	},
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_pc"] = "+em_pc",
		["$(INSDIR)/share/man/man6/em_pc.6"] = "./em_pc.6"
	}
}
