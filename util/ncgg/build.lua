include("first/bison.lua")

local cggparser = bison {
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
		"./make_enterkeyw_c.lua",
		"./keywords",
	},
	outleaves = { "enterkeyw.c" },
	commands = {
		"$(LUA) %{ins[1]} < %{ins[2]} > %{outs[1]}"
	}
}

cprogram {
	name = "ncgg",
	srcs = concat(
		"./coerc.c",
		"./emlookup.c",
		"./error.c",
		"./expr.c",
		"./hall.c",
		"./instruct.c",
		"./iocc.c",
		"./lookup.c",
		"./main.c",
		"./output.c",
		"./set.c",
		"./strlookup.c",
		"./subr.c",
		"./var.c",
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
