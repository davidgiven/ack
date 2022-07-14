include("util/LLgen/build.lua")
include("util/cmisc/build.lua")

normalrule {
	name = "parameters",
	ins = { "./BigPars" },
	outleaves = { "parameters.h" },
	commands = {
		"echo '#ifndef PARAMETERS_H' > %{outs}",
		"echo '#define PARAMETERS_H' >> %{outs}",
		"grep -v '^!' < %{ins} >> %{outs}",
		"echo '#endif' >> %{outs}"
	}
}

local str_files = {
		"./code.str",
		"./declar.str",
		"./def.str",
		"./estack.str",
		"./expr.str",
		"./field.str",
		"./idf.str",
		"./l_brace.str",
		"./l_outdef.str",
		"./l_state.str",
		"./macro.str",
		"./next.str",
		"./proto.str",
		"./stack.str",
		"./stmt.str",
		"./struct.str",
		"./switch.str",
		"./type.str",
		"./util.str"
}

local str_bases = basename(filenamesof(str_files))

local str_targets = {}

for _, f in ipairs(str_bases) do
	local bf = f:gsub("%.str$", ""):gsub("^$./", "")
	str_targets[#str_targets+1] = normalrule {
		name = "allocd_header/"..bf,
		ins = { "./make.allocd", "./"..f },
		outleaves = { bf..".h" },
		commands = {
			"%{ins[1]} < %{ins[2]} > %{outs}"
		}
	}
end

normalrule {
	name = "next-c",
	ins = {
		"./make.next",
		str_files
	},
	outleaves = { "next.c" },
	commands = {
		"%{ins} > %{outs[1]}"
	}
}

clibrary {
	name = "nextlib",
	srcs = { "+next-c" },
	hdrs = str_targets,
	deps = {
		"+parameters",
		str_targets
	}
}

normalrule {
	name = "tokenfile-g",
	ins = {
		"./make.tokfile",
		"./tokenname.c",
	},
	outleaves = { "tokenfile.g" },
	commands = {
		"sh %{ins[1]} < %{ins[2]} > %{outs}",
	}
}

normalrule {
	name = "symbol2str-c",
	ins = {
		"./make.tokcase",
		"./tokenname.c",
	},
	outleaves = { "symbol2str.c" },
	commands = {
		"sh %{ins[1]} < %{ins[2]} > %{outs}",
	}
}

llgen {
	name = "llgen",
	srcs = {
		"+tokenfile-g", -- must be first
		"./declar.g",
		"./expression.g",
		"./ival.g",
		"./program.g",
		"./statement.g",
	},
}

tabgen {
	name = "tabgen",
	srcs = { "./char.tab" }
}

cprogram {
	name = "cemcom",
	srcs = {
		"./LLlex.c",
		"./LLmessage.c",
		"./arith.c",
		"./blocks.c",
		"./ch3.c",
		"./ch3bin.c",
		"./ch3mon.c",
		"./code.c",
		"./conversion.c",
		"./cstoper.c",
		"./dataflow.c",
		"./declarator.c",
		"./decspecs.c",
		"./domacro.c",
		"./dumpidf.c",
		"./error.c",
		"./eval.c",
		"./expr.c",
		"./field.c",
		"./fltcstoper.c",
		"./idf.c",
		"./input.c",
		"./l_comment.c",
		"./l_ev_ord.c",
		"./l_lint.c",
		"./l_misc.c",
		"./l_outdef.c",
		"./l_states.c",
		"./label.c",
		"./main.c",
		"./options.c",
		"./pragma.c",
		"./proto.c",
		"./skip.c",
		"./stab.c",
		"./stack.c",
		"./struct.c",
		"./switch.c",
		"./tokenname.c",
		"./type.c",
		"./util.c",
		"+symbol2str-c",
		"+tabgen",
		matching(filenamesof("+llgen"), "%.c$"),
	},
	deps = {
		"./LLlex.h", "./align.h", "./arith.h", "./atw.h",
		"./class.h", "./decspecs.h", "./file_info.h",
		"./input.h", "./interface.h", "./l_class.h",
		"./l_comment.h", "./l_em.h", "./l_lint.h",
		"./label.h", "./level.h", "./mes.h", "./sizes.h",
		"./specials.h", "./tokenname.h",
		"+llgen",
		"+nextlib",
		"+parameters",
		"h+emheaders",
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
		["$(PLATDEP)/em_cemcom.ansi"] = "+cemcom"
	}
}
