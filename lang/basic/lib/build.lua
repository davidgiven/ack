include("plat/build.lua")

for _, plat in ipairs(vars.emplats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"./*.c",
			"./*.e",
        },
		hdrs = {}, -- must be empty
		deps = {
			"h+emheaders",
			"lang/cem/libcc.ansi/headers+headers",
			"plat/"..plat.."/include+headers",
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

