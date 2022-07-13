clibrary {
	name = "headers",
	hdrs = {
		"./src/cclass.h",
		"./src/extern.h",
		"./src/io.h",
		"./src/Lpars.h",
		"./src/sets.h",
		"./src/types.h",
	}
}

cprogram {
	name = "llgen",

	-- These use pre-LLgen'd versions of LLgen.c, Lpars.c, Lpars.h, and
	-- tokens.c. If LLgen.g or tokens.g gets updated, they need
	-- rebuilding. Use the bootstrap target to do this.

	srcs = {
		"./src/LLgen.c",
		"./src/Lpars.c",
		"./src/alloc.c",
		"./src/cclass.c",
		"./src/check.c",
		"./src/compute.c",
		"./src/gencode.c",
		"./src/global.c",
		"./src/machdep.c",
		"./src/main.c",
		"./src/name.c",
		"./src/reach.c",
		"./src/savegram.c",
		"./src/sets.c",
		"./src/tokens.c",
		"./src/utils.c",
	},
	deps = { "+headers" },
	vars = {
		["+cflags"] = {
			"-DLIBDIR=\\\""..cwd().."/lib\\\"",
			"-DNON_CORRECTING"
		},
	}
}

-- This bootstrap target rebuilds LLgen's own parser with LLgen.
-- It acts like ./bootstrap.sh but without installing LLgen in PATH.
normalrule {
	name = "bootstrap",
	ins = "+llgen",
	outleaves = { "phony" },
	commands = {
		"cd %{abspath(cwd()..\"/src\")}",
		"%{abspath(ins)} -vvv -x tokens.g LLgen.g",
		"echo",
		"echo You should now be able to rebuild LLgen with the new parser.",
	}
}

definerule("llgen",
	{
		srcs = { type="targets" },
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		local fs = replace(basename(filenamesof(e.srcs)), "%.g$", "")

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			outleaves = {
				"Lpars.c",
				"Lpars.h",
				replace(fs, "$", ".c")
			},
			ins = {
				"util/LLgen+llgen",
				e.srcs,
			},
			vars = {
				srcs = e.srcs
			},
			commands = {
				"rm -f %{outs} && %{ins[1]} -o%{dirname(outs[1])} %{srcs}"
			}
		}
	end
)
