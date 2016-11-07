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
	},
	hdrs = { "./flt_arith.h" },
	deps = {
		"modules+headers",
		"./*.h",
	}
}

-- The test program isn't built by default.  Here is a target
-- modules/src/flt_arith+pkg to install it as flt_test

cprogram {
	name = "test",
	srcs = { "./test.c" },
	deps = { "+lib" },
}

installable {
	name = "pkg",
	map = {
		["$(INSDIR)/bin/flt_test"] = "+test",
	},
}
