include("plat/build.lua")

local installmap = {}

local function addheader(dir, list)
	for _, f in ipairs(list) do
		local b = basename(f)
		installmap[concatpath("$(PLATIND)/include/modula2/", dir, b)] = f
	end
end

addheader("", filenamesof("./*.def"))

installable {
	name = "headers",
	map = installmap
}

for _, plat in ipairs(vars.plats) do
    acklibrary {
        name = "lib_"..plat,
        srcs = {
			"./*.c",
			"./*.mod",
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


