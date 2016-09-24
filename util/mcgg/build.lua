include("first/yacc.lua")

flex {
	name = "flex",
	srcs = { "./*.l" },
}

yacc {
	name = "yacc",
	srcs = { "./*.y" },
}

cprogram {
	name = "mcgg",
	srcs = {
		"./*.c",
		matching(filenamesof("+flex"), "%.c$"),
		matching(filenamesof("+yacc"), "%.c$")
	},
	deps = {
		"./*.h",
		"+yacc"
	}
}
	
definerule("mcgg",
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
			},
			ins = {
				"util/mcgg+mcgg",
				cpptable
			},
			commands = {
				"%{ins[1]} < %{ins[2]} > %{outs}",
			}
		}
	end
)

