include("plat/build.lua")

-- For now, all floats are little-endian.
local byte_order = "mach/i86/libfp/byte_order.h"

local srcs = {
	"./adder.c",
	"./add_ext.c",
	"./adf4.c",
	"./adf8.c",
	"./cff4.c",
	"./cff8.c",
	"./cfi.c",
	"./cfu.c",
	"./cif4.c",
	"./cif8.c",
	"./cmf4.c",
	"./cmf8.c",
	"./compact.c",
	"./cuf4.c",
	"./cuf8.c",
	"./div_ext.c",
	"./dvf4.c",
	"./dvf8.c",
	"./extend.c",
	"./fef4.c",
	"./fef8.c",
	"./fif4.c",
	"./fif8.c",
	"./fptrp.e",
	"./mlf4.c",
	"./mlf8.c",
	"./mul_ext.c",
	"./ngf4.c",
	"./ngf8.c",
	"./nrm_ext.c",
	"./sbf4.c",
	"./sbf8.c",
	"./sft_ext.c",
	"./shifter.c",
	"./sub_ext.c",
	"./zrf4.c",
	"./zrf8.c",
	"./zrf_ext.c",
}

-- For now, only cpm uses software floating-point.
for _, plat in ipairs({"cpm"}) do
	local edits = {}
	for _, src in fpairs(srcs) do

		-- Compile each src file into assembly code.
		local n = basename(src):gsub("%.%w*$", "")
		local assembly = ackfile {
			name = "s_"..plat.."/"..n,
			srcs = { src },
			deps = {
				"./FP_bias.h",
				"./FP_shift.h",
				"./FP_trap.h",
				"./FP_types.h",
				"./get_put.h",
				byte_order,
			},
			suffix = ".s",
			vars = {
				["+ackcflags"] = { "-I"..dirname(byte_order) },
				plat = plat
			}
		}

		-- Run massage_s.lua to edit the assembly code.
		edits[#edits+1] = normalrule {
			name = "ed_"..plat.."/"..n,
			ins = {
				"./massage_s.lua",
				assembly,
			},
			outleaves = { n..".s" },
			commands = {
				"$(LUA) %{ins[1]} <%{ins[2]} >%{outs}"
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
