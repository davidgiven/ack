include("plat/build.lua")

local installmap = {}

local function addheader(dir, list)
	for _, f in ipairs(list) do
		local b = basename(f)
		installmap[concatpath("$(PLATIND)/include/modula2/", dir, b)] = f
	end
end

addheader("", filenamesof(
		"./Arguments.def",
		"./ArraySort.def",
		"./ASCII.def",
		"./Conversions.def",
		"./CSP.def",
		"./EM.def",
		"./Epilogue.def",
		"./InOut.def",
		"./MathLib0.def",
		"./Mathlib.def",
		"./PascalIO.def",
		"./Processes.def",
		"./random.def",
		"./RealConversions.def",
		"./RealInOut.def",
		"./Semaphores.def",
		"./Storage.def",
		"./Streams.def",
		"./Strings.def",
		"./StripUnix.def",
		"./Termcap.def",
		"./Terminal.def",
		"./Traps.def",
		"./Unix.def",
		"./XXTermcap.def"
	))


installable {
	name = "headers",
	map = installmap
}

for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"./absd.c",
			"./absi.c",
			"./absl.c",
			"./Arguments.c",
			"./ArraySort.mod",
			"./blockmove.c",
			"./cap.c",
			"./catch.c",
			"./confarray.c",
			"./Conversions.mod",
			"./CSP.mod",
			"./dvi.c",
			"./halt.c",
			"./init.c",
			"./InOut.mod",
			"./load.c",
			"./MathLib0.mod",
			"./Mathlib.mod",
			"./PascalIO.mod",
			"./Processes.mod",
			"./random.mod",
			"./rcka.c",
			"./rcki.c",
			"./rckil.c",
			"./rcku.c",
			"./rckul.c",
			"./RealConversions.mod",
			"./RealInOut.mod",
			"./Semaphores.mod",
			"./sigtrp.c",
			"./stackprio.c",
			"./Storage.mod",
			"./store.c",
			"./StrAss.c",
			"./Streams.mod",
			"./Strings.mod",
			"./SYSTEM.c",
			"./Termcap.mod",
			"./Terminal.mod",
			"./Traps.mod",
			"./ucheck.c",
			"./EM.e",
			"./LtoUset.e",
			"./absf.e",
			"./par_misc.e",
        },
		hdrs = {}, -- must be empty
		deps = {
			"lang/cem/libcc.ansi/headers+pkg",
			"plat/"..plat.."/include+pkg",
			"h+emheaders",
			"lang/m2/include+headers",
		},
        vars = { plat = plat }
    }

	ackfile {
		name = "mrt_"..plat,
		srcs = { "./head_m2.e" },
		vars = { plat = plat },
		deps = {
			"h+emheaders"
		}
	}

	local suffix = plat:find("^em") and "m" or "o"
	installable {
		name = "pkg_"..plat,
		map = {
			["$(PLATIND)/"..plat.."/modula2."..suffix] = "+mrt_"..plat,
			["$(PLATIND)/"..plat.."/libmodula2.a"] = "+lib_"..plat,
			"+headers",
		}
	}
end


