include("plat/build.lua")

for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"./abs.c",
			"./asc.c",
			"./asrt.c",
			"./atn.c",
			"./chr.c",
			"./conversion.c",
			"./error.c",
			"./exp.c",
			"./file.c",
			"./hlt.c",
			"./io.c",
			"./log.c",
			"./mki.c",
			"./oct.c",
			"./peek.c",
			"./power.c",
			"./print.c",
			"./random.c",
			"./read.c",
			"./return.c",
			"./salloc.c",
			"./sgn.c",
			"./sin.c",
			"./sqt.c",
			"./stop.c",
			"./string.c",
			"./swap.c",
			"./trace.c",
			"./trap.c",
			"./write.c",
			"./fef.e",
			"./fif.e",
			"./setline.e",
        },
		hdrs = {}, -- must be empty
		deps = {
			"h+emheaders",
			"lang/cem/libcc.ansi/headers+pkg",
			"plat/"..plat.."/include+pkg",
			"./bc_io.h",
			"./lib.h",
		},
        vars = { plat = plat }
    }

	installable {
		name = "pkg_"..plat,
		map = {
			["$(PLATIND)/"..plat.."/libbasic.a"] = "+lib_"..plat,
		}
	}
end

