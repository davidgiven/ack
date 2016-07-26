cprogram {
	name = "llgen",

	-- These use pre-LLgen'd versions of LLgen.c, Lpars.c and tokens.c. If
	-- LLgen.g gets updated, they need rebuilding. Use the bootstrap script to
	-- do this.

	srcs = { "./src/*.c" },
	vars = {
		["+cflags"] = {
			"-DLIBDIR=\\\""..posix.getcwd().."/"..cwd().."/lib\\\"",
			"-DNON_CORRECTING"
		},
	}
}

definerule("llgen",
	{
		srcs = { type="targets" },
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		local fs = replace(basename(e.srcs), "%.g$", "")

		return normalrule {
			name = e.name,
			cwd = e.cwd,
			outleaves = {
				"Lpars.c",
				"Lpars.h",
				unpack(replace(fs, "$", ".c"))
			},
			ins = {
				"util/LLgen+llgen",
				unpack(e.srcs),
			},
			commands = {
				"cd %{dir} && %{abspath(ins)}"
			}
		}
	end
)
