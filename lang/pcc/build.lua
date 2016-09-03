include("first/yacc.lua")

flex {
	name = "flex",
	srcs = { "./pcc/cc/ccom/scan.l" }
}

yacc {
	name = "yacc",
	srcs = { "./pcc/cc/ccom/cgram.y" }
}

normalrule {
	name = "cgram_h",
	ins = { matching(filenamesof("+yacc"), "%.h$") },
	outleaves = { "cgram.h" },
	commands = {
		"cp %{ins} %{outs}"
	}
}

clibrary {
	name = "config_h",
	srcs = {},
	hdrs = { "./config.h" }
}

definerule("build_pcc",
	{
		pccarch = { type="string" }
	},
	function(e)
		local pccarch = e.pccarch

		local pcc_cflags = {
			"-D_DEFAULT_SOURCE",
			"-DGCC_COMPAT",
			"-DPCC_DEBUG",
			"-DNATIVE_FLOATING_POINT",
			"-D_ISOC99_SOURCE",
			"-Dos_ack",
			"-Dmach_"..pccarch
		}

		local headers = clibrary {
			name = e.name.."/headers",
			srcs = {},
			hdrs = {
				"lang/pcc/pcc/arch/"..pccarch.."/macdefs.h",
				"lang/pcc/pcc/cc/ccom/pass1.h",
				"lang/pcc/pcc/common/*.h",
				"lang/pcc/pcc/mip/*.h",
			}
		}

		local mkext = cprogram {
			name = e.name.."/mkext",
			srcs = {
				"lang/pcc/pcc/mip/mkext.c",
				"lang/pcc/pcc/mip/common.c",
				"lang/pcc/pcc/arch/"..pccarch.."/table.c"
			},
			deps = {
				headers,
				"lang/pcc+config_h",
			},
			vars = {
				["+cflags"] = { "-DMKEXT", unpack(pcc_cflags) }
			}
		}

		local external = normalrule {
			name = e.name.."/external",
			ins = { mkext },
			outleaves = { "external.h", "external.c" },
			commands = {
				"(cd %{dir} && %{ins})"
			}
		}

		return cprogram {
			name = e.name,
			srcs = {
				matching(filenamesof(external), "%.c$"),
				matching(filenamesof("lang/pcc+yacc", "lang/pcc+flex"), "%.c$"),
				"lang/pcc/pcc/arch/"..pccarch.."/*.c",
				"lang/pcc/pcc/cc/ccom/*.c",
				"lang/pcc/pcc/common/*.c",
				"lang/pcc/pcc/mip/common.c",
				"lang/pcc/pcc/mip/match.c",
				"lang/pcc/pcc/mip/optim2.c",
				"lang/pcc/pcc/mip/reader.c",
				"lang/pcc/pcc/mip/regs.c",
			},
			deps = {
				headers,
				external,
				"lang/pcc+cgram_h",
				"lang/pcc+config_h",
			},
			vars = {
				["+cflags"] = pcc_cflags
			}
		}
	end
)

