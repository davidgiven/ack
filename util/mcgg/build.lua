include("first/yacc.lua")

flex {
	name = "flex",
	srcs = { "./*.l" },
}

yacc {
	name = "yacc",
	srcs = { "./*.y" },
}

normalrule {
	name = "ircodes",
	outleaves = { "ircodes-dyn.h", "ircodes.c" },
	ins = {
		"./ircodes.sh",
		"./ir.dat"
	},
	commands = {
		"%{ins[1]} %{ins[2]} %{outs[1]} %{outs[2]}"
	}
}

clibrary {
	name = "lib",
	srcs = {
		matching(filenamesof("+ircodes"), "%.c$"),
	},
	deps = {
		"+ircodes",
		"./ircodes.h"
	},
	hdrs = {
		matching(filenamesof("+ircodes"), "%.h$"),
		"./ircodes.h",
		"./mcgg.h"
	}
}

cprogram {
	name = "mcgg",
	srcs = {
		"./*.c",
		matching(filenamesof("+flex"), "%.c$"),
		matching(filenamesof("+yacc"), "%.c$")
	},
	deps = {
		"./iburg.h",
		"+lib",
		"+yacc",
		"modules/src/data+lib",
		"modules+headers",
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
				"tables.h",
			},
			ins = {
				"util/mcgg+mcgg",
				cpptable
			},
			commands = {
				"%{ins[1]} -i %{ins[2]} -o %{outs[1]} -h %{outs[2]}",
			}
		}
	end
)

