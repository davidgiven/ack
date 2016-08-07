clibrary {
	name = "lib",
	srcs = {
		"./flt_ar2flt.c",
		"./flt_div.c",
		"./flt_flt2ar.c",
		"./flt_modf.c",
		"./flt_str2fl.c",
		"./flt_cmp.c",
		"./flt_add.c",
		"./b64_add.c",
		"./flt_mul.c",
		"./flt_nrm.c",
		"./b64_sft.c",
		"./flt_umin.c",
		"./flt_chk.c",
		"./split.c",
		"./ucmp.c",
	},
	hdrs = { "./flt_arith.h" },
	deps = {
		"modules+headers"
	}
}


