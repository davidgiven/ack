include("util/cgg/build.lua")

definerule("build_cg",
	{
		arch = { type="string" }
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		local headers = clibrary {
			name = e.name.."/headers",
			srcs = {},
			hdrs = {
				"mach/proto/cg/assert.h",
				"mach/proto/cg/codegen.h",
				"mach/proto/cg/data.h",
				"mach/proto/cg/equiv.h",
				"mach/proto/cg/extern.h",
				"mach/proto/cg/fillem.h",
				"mach/proto/cg/gencode.h",
				"mach/proto/cg/glosym.h",
				"mach/proto/cg/param.h",
				"mach/proto/cg/reg.h",
				"mach/proto/cg/regvar.h",
				"mach/proto/cg/result.h",
				"mach/proto/cg/salloc.h",
				"mach/proto/cg/state.h",
				"mach/proto/cg/subr.h",
				"mach/proto/cg/types.h",
				"mach/"..e.arch.."/cg/mach.c",
				"mach/"..e.arch.."/cg/mach.h",
			}
		}

		local tables = cgg {
			name = e.name.."/tables",
			srcs = { "mach/"..e.arch.."/cg/table" }
		}

		return cprogram {
			name = e.name,
			srcs = {
				"mach/proto/cg/codegen.c",
				"mach/proto/cg/compute.c",
				"mach/proto/cg/equiv.c",
				"mach/proto/cg/fillem.c",
				"mach/proto/cg/gencode.c",
				"mach/proto/cg/glosym.c",
				"mach/proto/cg/main.c",
				"mach/proto/cg/move.c",
				"mach/proto/cg/nextem.c",
				"mach/proto/cg/reg.c",
				"mach/proto/cg/regvar.c",
				"mach/proto/cg/salloc.c",
				"mach/proto/cg/state.c",
				"mach/proto/cg/subr.c",
				"mach/proto/cg/var.c",
				matching(filenamesof(tables), "%.c$")
			},
			deps = {
				"h+emheaders",
				"modules+headers",
				"modules/src/em_data+lib",
				"modules/src/flt_arith+lib",
				headers,
				tables, -- for .h file
			}
		}
	end
)

