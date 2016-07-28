include("mach/proto/as/build.lua")
include("mach/proto/ncg/build.lua")

definerule("ackfile",
	{
		srcs = { type="targets" },
		deps = { type="targets", default={} },
	},
	function (e)
		local plat = e.vars.plat

		return cfile {
			name = e.name,
			srcs = e.srcs,
			deps = {
				"plat/"..plat.."+tools",
				"util/ack+pkg",
				"lang/cem/cpp.ansi+pkg",
				unpack(e.deps)
			},
			commands = {
				"ACKDIR=$(INSDIR) $(INSDIR)/bin/ack -m%{plat} -c -o %{outs} %{ins} %{hdrpaths} %{ackcflags}"
			}
		}
	end
)


definerule("build_plat_tools",
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

		return installable {
			name = e.name,
			map = {
				["$(PLATDEP)/"..e.plat.."/as"] = as,
				["$(PLATDEP)/"..e.plat.."/ncg"] = ncg,
				["$(PLATIND)/descr/"..e.plat] = descr,
			}
		}
	end
)
