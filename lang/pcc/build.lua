include("first/yacc.lua")

local pccarch = "arm"

local pcc_cflags = {
	"-D_DEFAULT_SOURCE",
	"-DGCC_COMPAT",
	"-DPCC_DEBUG",
	"-DNATIVE_FLOATING_POINT",
	"-D_ISOC99_SOURCE",
	"-Dos_ack",
	"-Dmach_"..pccarch
}

clibrary {
	name = "headers",
	srcs = {},
	hdrs = {
		"./pcc/arch/"..pccarch.."/macdefs.h",
		"./pcc/cc/ccom/pass1.h",
		"./pcc/common/*.h",
		"./pcc/mip/*.h",
	}
}

cprogram {
	name = "mkext",
	srcs = {
		"./pcc/mip/mkext.c",
		"./pcc/mip/common.c",
		"./pcc/arch/"..pccarch.."/table.c"
	},
	deps = {
		"+headers"
	},
	vars = {
		["+cflags"] = { "-DMKEXT", unpack(pcc_cflags) }
	}
}

normalrule {
	name = "external",
	ins = { "+mkext" },
	outleaves = { "external.h", "external.c" },
	commands = {
		"(cd %{dir} && %{ins})"
	}
}

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

cprogram {
	name = "pcc_ccom",
	srcs = {
		matching(filenamesof("+external"), "%.c$"),
		matching(filenamesof("+yacc", "+flex"), "%.c$"),
		"./pcc/arch/"..pccarch.."/*.c",
		"./pcc/cc/ccom/*.c",
		"./pcc/common/*.c",
		"./pcc/mip/common.c",
		"./pcc/mip/match.c",
		"./pcc/mip/optim2.c",
		"./pcc/mip/reader.c",
		"./pcc/mip/regs.c",
	},
	deps = {
		"+headers",
		"+external",
		"+cgram_h",
	},
	vars = {
		["+cflags"] = pcc_cflags
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/pcc_ccom"] = "+pcc_ccom",
	}
}

