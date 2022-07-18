include("util/ncgg/build.lua")

definerule("build_ncg",
	{
		arch = { type="string" },
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		local headers = clibrary {
			name = e.name.."/headers",
			srcs = {},
			hdrs = {
				"mach/proto/ncg/data.h",
				"mach/proto/ncg/equiv.h",
				"mach/proto/ncg/glosym.h",
				"mach/proto/ncg/label.h",
				"mach/proto/ncg/param.h",
				"mach/proto/ncg/regvar.h",
				"mach/proto/ncg/result.h",
				"mach/proto/ncg/state.h",
				"mach/proto/ncg/types.h",
				"mach/proto/ncg/xmach.h",
				"mach/"..e.arch.."/ncg+headers",
			}
		}

		local tables = ncgg {
			name = e.name.."/tables",
			srcs = { "mach/"..e.arch.."/ncg/table" }
		}

		return cprogram {
			name = e.name,
			srcs = {
				"mach/proto/ncg/codegen.c",
				"mach/proto/ncg/compute.c",
				"mach/proto/ncg/equiv.c",
				"mach/proto/ncg/fillem.c",
				"mach/proto/ncg/gencode.c",
				"mach/proto/ncg/glosym.c",
				"mach/proto/ncg/label.c",
				"mach/proto/ncg/main.c",
				"mach/proto/ncg/move.c",
				"mach/proto/ncg/nextem.c",
				"mach/proto/ncg/reg.c",
				"mach/proto/ncg/regvar.c",
				"mach/proto/ncg/salloc.c",
				"mach/proto/ncg/state.c",
				"mach/proto/ncg/subr.c",
				"mach/proto/ncg/var.c",
				matching(filenamesof(tables), "%.c$")
			},
			deps = {
				"h+emheaders",
				"modules+headers",
				"modules/src/flt_arith+lib",
				"modules/src/object+lib",
				"modules/src/em_data+lib",
				headers,
				tables, -- for .h file
			}
		}
	end
)

