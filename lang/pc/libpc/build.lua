include("plat/build.lua")

for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"./*.c",
			"./bts.e",
			"./encaps.e",
			"./fef.e",
			"./fif.e",
			"./gto.e",
			"./hol0.e",
			"./sig.e",
			"./trap.e",
			"./trp.e",
        },
		hdrs = {}, -- must be empty
		deps = {
			"lang/cem/libcc.ansi/headers+pkg",
			"plat/"..plat.."/include+pkg",
			"h+emheaders",
		},
        vars = { plat = plat }
    }

	ackfile {
		name = "prt_"..plat,
		srcs = { "./head_pc.e" },
		vars = { plat = plat },
		deps = {
			"h+emheaders"
		}
	}

	installable {
		name = "pkg_"..plat,
		map = {
			["$(PLATIND)/"..plat.."/pascal.o"] = "+prt_"..plat,
			["$(PLATIND)/"..plat.."/libpascal.a"] = "+lib_"..plat,
		}
	}
end


