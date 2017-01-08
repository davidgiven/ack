include("plat/build.lua")

local conly = {
	rpi = true
}

local sourcefiles = filenamesof(
	"./hilo.b",
	"./hilo.bas",
	"./hilo.c",
	"./hilo.mod",
	"./hilo.p",
	"./mandelbrot.c",
	"./paranoia.c",
	"./startrek.c"
)

local installmap = {}
for _, file in ipairs(sourcefiles) do
	local b = basename(file)
	local be = replace(b, "%.", "_")
	local _, _, e = b:find("%.(%w*)$")

	for _, plat in ipairs(vars.plats) do
		if (e == "c") or not conly[plat] then
			local exe = ackprogram {
				name = be.."_"..plat,
				srcs = { file },
				vars = {
					plat = plat,
					lang = e,
				}
			}

			installmap["$(PLATIND)/examples/"..be.."."..plat] = exe
		end
	end
	installmap["$(PLATIND)/examples/"..b] = file
end

installable {
	name = "pkg",
	map = installmap
}

