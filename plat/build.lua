include("mach/proto/as/build.lua")
include("mach/proto/ncg/build.lua")

definerule("build_plat",
	{
		arch = { type = "string" },
		plat = { type = "string" },
	},
	function(e)
		local descr = "plat/"..e.plat.."/descr"

		local as = build_as {
			name = "as",
			arch = e.arch
		}

		local ncg = build_ncg {
			name = "ncg",
			arch = e.arch,
		}

		local tools = installable {
			name = "tools",
			map = {
				["$(PLATDEP)/"..e.arch.."/as"] = as,
				["$(PLATDEP)/"..e.plat.."/ncg"] = ncg,
				["$(PLATIND)/descr/"..e.plat.."/descr"] = descr,
			}
		}

		local libraries = installable {
			name = "libraries",
			map = {
			}
		}

		return installable {
			name = e.name,
			map = {
				tools,
				libraries,
			}
		}
	end
)

