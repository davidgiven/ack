include("first/yacc.lua")

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
			hdrs = {
				"mach/"..e.arch.."/as/mach*.c",
				"mach/"..e.arch.."/as/*.h"
			}
		}

		local preprocessedy = cppfile {
			name = e.name.."/yaccinput",
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

		local yaccfiles = yacc {
			name = e.name.."/yacc",
			srcs = { preprocessedy }
		}

		return cprogram {
			name = e.name,
			srcs = concat(
				"mach/proto/as/*.c",
				matching(filenamesof(yaccfiles), "%.c$")
			),
			deps = {
				"h+emheaders",
				"modules/src/object+lib",
				archlib,
				yaccfiles,
				e.deps
			}
		}
	end
)

