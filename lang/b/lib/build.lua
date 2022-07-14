include("plat/build.lua")

for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"./init.c",
			"./main.c",
        },
		hdrs = {}, -- must be empty
		deps = {
			"./b.h",
			"h+emheaders",
			"lang/cem/libcc.ansi/headers+pkg",
			"plat/"..plat.."/include+pkg",
		},
        vars = { plat = plat }
    }

	installable {
		name = "pkg_"..plat,
		map = {
			["$(PLATIND)/"..plat.."/libb.a"] = "+lib_"..plat,
		}
	}
end

