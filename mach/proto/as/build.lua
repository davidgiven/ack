include("first/yacc.lua")

definerule("build_as",
	{
		arch = { type="string" }
	},
	function(e)
		-- Remember this is executed from the caller's directory; local
		-- target names will resolve there
		local archlib = clibrary {
			name = e.name.."/archlib",
			srcs = {},
			hdrs = { "mach/"..e.arch.."/as/mach*.c" }
		}

		local preprocessedy = cppfile {
			name = e.name.."/yaccinput",
			srcs = { "mach/proto/as/comm2.y" },
			outleaf = "comm2.y",
			deps = {
				"h+emheaders",
				archlib,
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
				filenamesof(yaccfiles, "%.c$")
			),
			deps = {
				"h+emheaders",
				"modules/src/object+lib",
				archlib,
				yaccfiles
			}
		}
	end
)

