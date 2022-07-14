include("first/yacc.lua")

local cggparser = yacc {
	name = "cggparser",
	srcs = { "./bootgram.y" }
}

local cgglexer = flex {
	name = "cgglexer",
	srcs = { "./bootlex.l" }
}

cprogram {
	name = "cgg",
	srcs = concat(
		"./main.c",
		matching(filenamesof(cggparser), "%.c$"),
		matching(filenamesof(cgglexer), "%.c$")
	),
	deps = {
		"./booth.h",
		"+cggparser", -- for .h file
		"+cgglexer", -- for .h file
		"h+emheaders",
		"modules/src/em_data+lib",
	}
}
	
definerule("cgg",
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
				"util/cgg+cgg",
				cpptable
			},
			commands = {
				"cd %{dir} && %{ins}",
			}
		}
	end
)
