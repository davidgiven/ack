include("plat/build.lua")

-- For now, all floats are little-endian.
local byte_order = "mach/i86/libfp/byte_order.h"

-- For now, only cpm uses software floating-point.
for _, plat in ipairs({"cpm"}) do
	local edits = {}
	for _, src in fpairs("./*.c", "./*.e") do

		-- Compile each src file into assembly code.
		local n = basename(src):gsub("%.%w*$", "")
		local assembly = ackfile {
			name = "s_"..plat.."/"..n,
			srcs = { src },
			deps = {
				"./*.h",
				byte_order,
			},
			suffix = ".s",
			vars = {
				["+ackcflags"] = { "-I"..dirname(byte_order) },
				plat = plat
			}
		}

		-- Run FP.script to edit the assembly code.
		edits[#edits+1] = normalrule {
			name = "ed_"..plat.."/"..n,
			ins = {
				"./FP.script",
				assembly,
			},
			outleaves = { n..".s" },
			commands = {
				"ed -s %{ins[2]} <%{ins[1]} >%{outs}"
			}
		}
	end

	acklibrary {
		name = "lib_"..plat,
		srcs = { edits },
		vars = { plat = plat }
	}

	installable {
		name = "pkg_"..plat,
		map = {
			["$(PLATIND)/"..plat.."/libfp.a"] = "+lib_"..plat,
		}
	}
end
