include("util/mcgg/build.lua")

definerule("build_mcg",
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
				"mach/proto/mcg/*.h",
				"mach/"..e.arch.."/mcg/*.h",
			}
		}

		local tables = mcgg {
			name = e.name.."/tables",
			srcs = { "mach/"..e.arch.."/mcg/table" }
		}

		return cprogram {
			name = e.name,
			srcs = {
				"mach/proto/mcg/*.c",
				"mach/"..e.arch.."/mcg/platform.c",
				matching(filenamesof(tables), "%.c$")
			},
			deps = {
				"h+emheaders",
				"modules+headers",
				"modules/src/alloc+lib",
				"modules/src/data+lib",
				"modules/src/em_code+lib_k",
				"modules/src/em_data+lib",
				"modules/src/idf+lib",
				"modules/src/object+lib",
				"modules/src/read_em+lib_kv",
				"modules/src/string+lib",
				"modules/src/system+lib",
				"util/mcgg+lib",
				headers,
				tables, -- for .h file
			}
		}
	end
)

