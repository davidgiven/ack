include("mach/proto/as/build.lua")
include("mach/proto/cg/build.lua")
include("mach/proto/ncg/build.lua")
include("mach/proto/mcg/build.lua")
include("mach/proto/top/build.lua")

definerule("ackfile",
	{
		srcs = { type="targets" },
		deps = { type="targets", default={} },
		suffix = { type="string", optional=true },
	},
	function (e)
		local em = (e.vars.plat or ""):find("^em")
		local suffix = e.suffix or (em and ".m" or ".o")
		local c = "-c"..suffix
		local plat = e.vars.plat

		return cfile {
			name = e.name,
			srcs = e.srcs,
			deps = {
				"lang/b/compiler+pkg",
				"lang/basic/src+pkg",
				"lang/cem/cemcom.ansi+pkg",
				"lang/cem/cpp.ansi+pkg",
				"lang/m2/comp+pkg",
				"lang/pc/comp+pkg",
				"plat/"..plat.."+tools",
				"util/ack+pkg",
				"util/ego+pkg",
				"util/misc+pkg",
				e.deps
			},
			suffix = suffix,
			commands = {
				"ACKDIR=$(INSDIR) $(INSDIR)/bin/ack -m%{plat} "..c.." -o %{outs} %{ins} %{hdrpaths} %{ackcflags}"
			}
		}
	end
)

definerule("acklibrary",
	{
		srcs = { type="targets", default={} },
		hdrs = { type="targets", default={} },
		deps = { type="targets", default={} },
	},
	function (e)
		local em = (e.vars.plat or ""):find("^em")

		local function splitter(srcs, num, cmd)
			local commands = {
			}
			local t = {}
			local function flush()
				commands[#commands+1] = cmd:gsub("%%%%", asstring(t))
				t = {}
			end
			for _, target in ipairs(srcs) do
				t[#t+1] = target
				if #t == num then
					flush()
				end
			end
			if #t ~= 0 then
				flush()
			end
			return table.concat(commands, " && ")
		end

		return clibrary {
			name = e.name,
			srcs = e.srcs,
			hdrs = e.hdrs,
			deps = {
				"util/arch+pkg",
				e.deps
			},
			vars = {
				splitter = splitter
			},
			_cfile = ackfile,
			suffix = em and ".m" or ".o",
			commands = {
				"rm -f %{outs[1]}",
				"%{splitter(ins, 100, 'ACKDIR=$(INSDIR) $(INSDIR)/bin/aal qc '..tostring(outs[1])..' %%')}"
			}
		}
	end
)

definerule("ackprogram",
	{
		srcs = { type="targets", default={} },
		deps = { type="targets", default={} },
	},
	function (e)
		-- This bit is a hack. We *don't* want to link the language libraries here,
		-- because the ack driver is going to pick the appropriate library itself and
		-- we don't want more than one. But we still need to depend on them, so we use
		-- a nasty hack.

		local platstamp = normalrule {
			name = e.name.."/platstamp",
			ins = { "plat/"..e.vars.plat.."+pkg" },
			outleaves = { "stamp" },
			commands = { "touch %{outs}" }
		}

		return cprogram {
			name = e.name,
			srcs = e.srcs,
			deps = {
				platstamp,
				"util/ack+pkg",
				"util/led+pkg",
				e.deps
			},
			_clibrary = acklibrary,
			commands = {
				"ACKDIR=$(INSDIR) $(INSDIR)/bin/ack -m%{plat} -.%{lang} -o %{outs} %{ins} %{ackldflags}"
			}
		}
	end
)

definerule("build_plat_libs",
	{
		arch = { type="string" },
		plat = { type="string" },
		em = { type="boolean", default=false },
	},
	function(e)
		local installmap = {
			"lang/b/lib+pkg_"..e.plat,
			"lang/basic/lib+pkg_"..e.plat,
			"lang/cem/libcc.ansi+pkg_"..e.plat,
			"lang/m2/libm2+pkg_"..e.plat,
			"lang/pc/libpc+pkg_"..e.plat,
			"lang/b/lib+pkg_"..e.plat,
			["$(PLATIND)/"..e.plat.."/libem.a"] = "mach/"..e.arch.."/libem+lib_"..e.plat,
			["$(PLATIND)/"..e.plat.."/libend.a"] = "mach/"..e.arch.."/libend+lib_"..e.plat,
		}

		-- For now, only cpm uses software floating-point.
		if e.plat == "cpm" then
			installmap[#installmap+1] = "mach/proto/fp+pkg_"..e.plat
		end

		return installable {
			name = e.name,
			map = installmap,
		}
	end
)

