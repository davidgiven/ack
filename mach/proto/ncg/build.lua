include("util/ncgg/build.lua")

definerule("build_ncg",
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
				"mach/proto/ncg/*.h",
				"mach/"..e.arch.."/ncg/mach*"
			}
		}

		local tables = ncgg {
			name = e.name.."/tables",
			srcs = { "mach/"..e.arch.."/ncg/table" }
		}

		return cprogram {
			name = e.name,
			srcs = {
				"mach/proto/ncg/*.c",
				"mach/"..e.arch.."/ncg/mach.h",
				archlib, -- for .c file
				tables, -- for .c file
			},
			deps = {
				"h+emheaders",
				"modules+headers",
				"modules/src/flt_arith+lib",
				"modules/src/object+lib",
				"util/data+em_data",
				headers,
				tables, -- for .h file
			}
		}
	end
)

