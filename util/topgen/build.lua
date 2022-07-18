include("util/LLgen/build.lua")

llgen {
	name = "llgen",
	srcs = {
		"./topgen.g",
	}
}

cprogram {
	name = "topgen",
	srcs = {
		"./hash.c",
		"./LLlex.c",
		"./main.c",
		"./pattern.c",
		"./symtab.c",
		matching(filenamesof("+llgen"), "%.c$"),
	},
	deps = {
		"+llgen",
	}
}

definerule("topgen",
	{
		srcs = { type="targets" },
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there

		return normalrule {
			name = e.name,
			outleaves = {
				"gen.c",
				"gen.h",
			},
			ins = {
				"util/topgen+topgen",
				e.srcs,
			},
			commands = {
				"%{ins[1]} %{ins[2]} %{dir}"
			}
		}
	end
)

