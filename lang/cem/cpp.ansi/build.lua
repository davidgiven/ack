include("util/cmisc/build.lua")
include("util/LLgen/build.lua")

local allocd_header = definerule(nil,
	{
		srcs = { type="targets" }
	},
	function (e)
		return normalrule {
			name = e.name,
			ins = {
				"./make.allocd",
				e.srcs
			},
			outleaves = basename(filenamesof(e.srcs)[1]):gsub("%.str$", ".h"),
			commands = {
				"%{ins[1]} < %{ins[2]} > %{outs}"
			}
		}
	end
)

allocd_header {
	name = "macro_h",
	srcs = { "./macro.str" }
}

allocd_header {
	name = "replace_h",
	srcs = { "./replace.str" }
}

normalrule {
	name = "tokenfile_g",
	ins = {
		"./make.tokfile",
		"./tokenname.c",
	},
	outleaves = { "tokenfile.g" },
	commands = {
		"sh %{ins[1]} < %{ins[2]} > %{outs}"
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
		"sh %{ins[1]} < %{ins[2]} > %{outs}"
	}
}

normalrule {
	name = "next_c",
	ins = {
		"./make.next",
		"./*.str",
	},
	outleaves = { "next.c" },
	commands = {
		"sh %{ins} > %{outs}"
	}
}

local llgen = llgen {
	name = "llgen",
	srcs = {
		"+tokenfile_g",
		"./expression.g"
	}
}

tabgen {
	name = "tabgen_c",
	srcs = { "./char.tab" }
}

cprogram {
	name = "cpp",
	srcs = concat(
		"./*.c",
		matching(filenamesof(llgen), "%.c$"),
		"+next_c",
		"+symbol2str_c",
		"+tabgen_c"
	),
	deps = {
		"./LLlex.h", "./arith.h", "./bits.h", "./class.h",
		"./file_info.h", "./idf.h", "./input.h",
		"./parameters.h",
		"+llgen",
		"+macro_h",
		"+replace_h",
		"modules+headers",
		"modules/src/alloc+lib",
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
		["$(PLATDEP)/cpp.ansi"] = "+cpp",
		["$(INSDIR)/share/man/man6/cpp.ansi.6"] = "./ncpp.6",
	}
}

