clibrary {
	name = "headers",
	hdrs = { "./src/*.h" } -- rm alloc.h
}

cprogram {
	name = "llgen",

	-- These use pre-LLgen'd versions of LLgen.c, Lpars.c, Lpars.h, and
	-- tokens.c. If LLgen.g or tokens.g gets updated, they need
	-- rebuilding. Use the bootstrap target to do this.

	srcs = { "./src/*.c" },
	deps = { "+headers" },
	vars = {
		["+cflags"] = {
			"-DLIBDIR=\\\""..posix.getcwd().."/"..cwd().."/lib\\\"",
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
			commands = {
				"cd %{dir} && rm -f %{outs} && %{abspath(ins)}"
			}
		}
	end
)
