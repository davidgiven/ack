normalrule {
	name = "em_code_ek_h",
	ins = {
		"./make.em.gen",
		"./em.nogen",
		"h/em_table"
	},
	outleaves = { "em_codeEK.h" },
	commands = {
		"%{ins[1]} %{ins[3]} > %{outs}",
		"cat %{ins[2]} >> %{outs}"
	}
}

clibrary {
    name = "headers",
    srcs = {},
    hdrs = {
        "+em_code_ek_h",
        "./em_code.h",
        "./em_codeCE.h",
        "./em_codeO.h",
    }
}

local function build_variant(code, cflags)
	clibrary {
		name = "lib_"..code,
		srcs = {
			"./C_out.c",
			"./bhcst.c",
			"./bhdlb.c",
			"./bhdnam.c",
			"./bhfcon.c",
			"./bhicon.c",
			"./bhilb.c",
			"./bhpnam.c",
			"./bhucon.c",
			"./crcst.c",
			"./crdlb.c",
			"./crdnam.c",
			"./crilb.c",
			"./crpnam.c",
			"./crscon.c",
			"./crxcon.c",
			"./cst.c",
			"./dfdlb.c",
			"./dfdnam.c",
			"./dfilb.c",
			"./dlb.c",
			"./dnam.c",
			"./em.c",
			"./end.c",
			"./endarg.c",
			"./exc.c",
			"./failed.c",
			"./fcon.c",
			"./getid.c",
			"./icon.c",
			"./ilb.c",
			"./insert.c",
			"./internerr.c",
			"./msend.c",
			"./msstart.c",
			"./op.c",
			"./opcst.c",
			"./opdlb.c",
			"./opdnam.c",
			"./opilb.c",
			"./opnarg.c",
			"./oppnam.c",
			"./pnam.c",
			"./pro.c",
			"./pronarg.c",
			"./psdlb.c",
			"./psdnam.c",
			"./pspnam.c",
			"./scon.c",
			"./ucon.c",
		},
		hdrs = {
            "+headers"
		},
		deps = {
			"+headers",
			"h+emheaders",
			"h+local",
			"modules+headers",
			"modules/src/alloc+lib",
			"modules/src/em_data+lib",
			"modules/src/system+lib",
		},
		vars = {
			["+cflags"] = cflags
		},
	}
end

build_variant("e", { "-DREADABLE_EM" })
build_variant("k", { })

