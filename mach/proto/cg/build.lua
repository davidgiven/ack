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
				"mach/proto/cg/*.h",
				"mach/"..e.arch.."/cg/mach.c",
				"mach/"..e.arch.."/cg/*.h",
			}
		}

		local tables = cgg {
			name = e.name.."/tables",
			srcs = { "mach/"..e.arch.."/cg/table" }
		}

		return cprogram {
			name = e.name,
			srcs = {
				"mach/proto/cg/*.c",
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

