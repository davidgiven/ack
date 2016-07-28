local posix = require("posix")
include("util/LLgen/build.lua")

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

local str_files = basename(posix.glob(cwd().."/*.str"))
local str_targets = {}

for _, f in ipairs(str_files) do
	local bf = f:gsub("%.str$", "")
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
		"./*.str",
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
		unpack(str_targets)
	}
}

normalrule {
	name = "tokenname-g",
	ins = {
		"./make.tokfile",
		"./tokenname.c",
	},
	outleaves = { "tokenname.g" },
	commands = {
		"sh %{ins[1]} < %{ins[2]} > %{outs}",
	}
}

llgen {
	name = "llgen",
	srcs = {
		"+tokenname-g", -- must be first
		"./*.g",
	},
}

cprogram {
	name = "cemcom",
	srcs = {
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
		"./init.c",
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
		"./replace.c",
		"./skip.c",
		"./stab.c",
		"./stack.c",
		"./struct.c",
		"./switch.c",
		"./tokenname.c",
		"./type.c",
		"./util.c",
	},
	deps = {
		"+parameters",
		"+nextlib",
		"+llgen",
		"h+emheaders",
		"modules/src/alloc+lib",
		"modules/src/em_code+lib_k",
		"modules/src/flt_arith+lib",
		"modules/src/idf+lib",
		"modules/src/input+lib",
		"modules/src/system+lib",
		"modules+headers",
		"util/data+em_data",
	},
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_cemcom.ansi"] = "+cemcom"
	}
}
