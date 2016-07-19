clibrary {
	name = "headers",
	hdrs = {
		"./h/*.h"
	}
}

clibrary {
	name = "system",
	srcs = { "./src/system/*.c" },
	hdrs = { "./src/system/system.h" },
	deps = { "+headers" },
}

clibrary {
	name = "alloc",
	srcs = { "./src/alloc/*.c" },
	hdrs = { "./src/alloc/alloc.h" },
	deps = {
		"+headers",
		"+system"
	},
}

clibrary {
	name = "flt_arith",
	srcs = { "./src/flt_arith/*.c" },
	hdrs = { "./src/flt_arith/flt_arith.h" },
	deps = {
		"+headers"
	}
}

clibrary {
	name = "idf",
	srcs = {},
	hdrs = { "./src/idf/idf_pkg.*" },
}

clibrary {
	name = "input",
	srcs = {
		"./src/input/*.c"
	},
	hdrs = { "./src/input/inp_pkg.*" }
}

clibrary {
	name = "string",
	srcs = { "./src/string/*.c" },
	deps = { "+headers" },
}

clibrary {
	name = "object",
	srcs = { "./src/object/*.c" },
	deps = {
		"+headers",
		"h+local",
		"h+emheaders",
	},
}

normalrule {
	name = "em_code_ek_h",
	ins = {
		"./src/em_code/make.em.gen",
		"./src/em_code/em.nogen",
		"h/em_table"
	},
	outleaves = { "em_codeEK.h" },
	commands = {
		"%{ins[1]} %{ins[3]} > %{outs}",
		"cat %{ins[2]} >> %{outs}"
	}
}

for _, variant in ipairs {
	{ "e", "-DREADABLE_EM" },
	{ "k", "" }
} do
	local code = variant[1]
	local cflags = variant[2]

	clibrary {
		name = "em_code_"..code,
		srcs = {
			"./src/em_code/C_out.c",
			"./src/em_code/bhcst.c",
			"./src/em_code/bhdlb.c",
			"./src/em_code/bhdnam.c",
			"./src/em_code/bhfcon.c",
			"./src/em_code/bhicon.c",
			"./src/em_code/bhilb.c",
			"./src/em_code/bhpnam.c",
			"./src/em_code/bhucon.c",
			"./src/em_code/crcst.c",
			"./src/em_code/crdlb.c",
			"./src/em_code/crdnam.c",
			"./src/em_code/crilb.c",
			"./src/em_code/crpnam.c",
			"./src/em_code/crscon.c",
			"./src/em_code/crxcon.c",
			"./src/em_code/cst.c",
			"./src/em_code/dfdlb.c",
			"./src/em_code/dfdnam.c",
			"./src/em_code/dfilb.c",
			"./src/em_code/dlb.c",
			"./src/em_code/dnam.c",
			"./src/em_code/em.c",
			"./src/em_code/end.c",
			"./src/em_code/endarg.c",
			"./src/em_code/exc.c",
			"./src/em_code/failed.c",
			"./src/em_code/fcon.c",
			"./src/em_code/getid.c",
			"./src/em_code/icon.c",
			"./src/em_code/ilb.c",
			"./src/em_code/insert.c",
			"./src/em_code/internerr.c",
			"./src/em_code/msend.c",
			"./src/em_code/msstart.c",
			"./src/em_code/op.c",
			"./src/em_code/opcst.c",
			"./src/em_code/opdlb.c",
			"./src/em_code/opdnam.c",
			"./src/em_code/opilb.c",
			"./src/em_code/opnarg.c",
			"./src/em_code/oppnam.c",
			"./src/em_code/pnam.c",
			"./src/em_code/pro.c",
			"./src/em_code/pronarg.c",
			"./src/em_code/psdlb.c",
			"./src/em_code/psdnam.c",
			"./src/em_code/pspnam.c",
			"./src/em_code/scon.c",
			"./src/em_code/ucon.c",
		},
		hdrs = {
			"+em_code_ek_h"
		},
		deps = {
			"+alloc",
			"+em_code_ek_h",
			"+headers",
			"+system",
			"h+emheaders",
			"h+local",
			"util/data+libem",
		},
		cflags = { cflags }
	}
end
