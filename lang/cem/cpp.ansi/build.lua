include("util/cmisc/build.lua")

local allocd_header = definerule(null,
	{
		srcs = { type="targets" }
	},
	function (e)
		return normalrule {
			name = e.name,
			ins = {
				"./make.allocd",
				unpack(e.srcs)
			},
			outleaves = replace(basename(e.srcs), "%.str$", ".h"),
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
		filenamesof(llgen, "%.c$"),
		"+next_c",
		"+symbol2str_c",
		"+tabgen_c"
	),
	deps = {
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
		["$(PLATDEP)/cpp.ansi"] = "+cpp"
	}
}

