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

for _, plat in ipairs(vars.emplats) do
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
			"lang/cem/libcc.ansi/headers+headers",
			"plat/"..plat.."/include+headers",
			"h+emheaders",
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

	installable {
		name = "pkg_"..plat,
		map = {
			["$(PLATIND)/"..plat.."/modula2.o"] = "+mrt_"..plat,
			["$(PLATIND)/"..plat.."/libmodula2.a"] = "+lib_"..plat,
			"+headers",
		}
	}
end


