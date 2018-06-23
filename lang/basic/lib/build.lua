include("plat/build.lua")

for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"./*.c",
			"./*.e",
        },
		hdrs = {}, -- must be empty
		deps = {
			"h+emheaders",
			"lang/cem/libcc.ansi/headers+pkg",
			"plat/"..plat.."/include+pkg",
			"./*.h",
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

