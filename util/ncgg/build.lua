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
	ins = concat(
		"./cvtkeywords",
		"./keywords",
		filenamesof({cggparser}, "%.h$")
	),
	outleaves = { "enterkeyw.c" },
	commands = {
		"%{ins[1]} %{ins[2]} %{ins[3]} %{outs[1]}"
	}
}

cprogram {
	name = "ncgg",
	srcs = concat(
		"./*.c",
		filenamesof({cggparser}, "%.c$"),
		filenamesof({cgglexer}, "%.c$"),
		"+keywords"
	),
	deps = {
		"+cggparser", -- for .h file
		"+cgglexer", -- for .h file
		"h+emheaders",
		"util/data+em_data",
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
