include("first/yacc.lua")

local cggparser = yacc {
	name = "cggparser",
	srcs = { "./cgg.y" }
}

local cgglexer = flex {
	name = "cgglexer",
	srcs = { "./scan.l" }
}

normalrule {
	name = "keywords",
	ins = {
		"./cvtkeywords",
		"util/cmisc+ed",
		"./keywords",
		matching(filenamesof(cggparser), "%.h$")
	},
	outleaves = { "enterkeyw.c" },
	commands = {
		"%{ins[1]} %{ins[2]} %{ins[3]} %{ins[4]} %{outs[1]}"
	}
}

cprogram {
	name = "ncgg",
	srcs = concat(
		"./*.c",
		matching(filenamesof(cggparser), "%.c$"),
		matching(filenamesof(cgglexer), "%.c$"),
		"+keywords"
	),
	deps = {
		"./cost.h", "./expr.h", "./extern.h", "./instruct.h",
		"./iocc.h", "./lookup.h", "./param.h", "./property.h",
		"./pseudo.h", "./reg.h", "./regvar.h", "./set.h",
		"./token.h", "./varinfo.h",
		"+cggparser", -- for .h file
		"+cgglexer", -- for .h file
		"h+emheaders",
		"modules/src/em_data+lib",
	}
}
	
definerule("ncgg",
	{
		srcs = { type="targets" }
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		if (#e.srcs ~= 1) then
			error("you must supply exactly one input file")
		end

		local cpptable = cppfile {
			name = e.name.."/cpptable",
			outleaf = "cpptable",
			srcs = e.srcs
		}

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			outleaves = {
				"tables.c",
				"tables.h",
			},
			ins = {
				"util/ncgg+ncgg",
				cpptable
			},
			commands = {
				"cd %{dir} && %{ins}",
				"mv %{dir}/tables.H %{dir}/tables.h"
			}
		}
	end
)
