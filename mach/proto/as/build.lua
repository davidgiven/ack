include("first/bison.lua")

definerule("build_as",
	{
		arch = { type="string" },
		deps = { type="targets", default={} },
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		local archlib = clibrary {
			name = e.name.."/archlib",
			srcs = {},
			hdrs = { "mach/"..e.arch.."/as+headers" },
		}

		local preprocessedy = cppfile {
			name = e.name.."/bisoninput",
			srcs = { "mach/proto/as/comm2.y" },
			outleaf = "comm2.y",
			deps = {
				"mach/proto/as/comm0.h",
				"mach/proto/as/comm1.h",
				"h+emheaders",
				archlib,
				e.deps,
			},
		}

		local bisonfiles = bison {
			name = e.name.."/bison",
			srcs = { preprocessedy }
		}

		return cprogram {
			name = e.name,
			srcs = concat(
				"mach/proto/as/comm3.c",
				"mach/proto/as/comm4.c",
				"mach/proto/as/comm5.c",
				"mach/proto/as/comm6.c",
				"mach/proto/as/comm7.c",
				"mach/proto/as/comm8.c",
				matching(filenamesof(bisonfiles), "%.c$")
			),
			deps = {
				"h+emheaders",
				"modules/src/object+lib",
				"modules/src/flt_arith+lib",
				archlib,
				bisonfiles,
				e.deps
			}
		}
	end
)

