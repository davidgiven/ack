cprogram {
	name = "esize",
	srcs = { "./esize.c" },
}

cprogram {
	name = "encode",
	srcs = { "./convert.c" },
	deps = {
		"h+emheaders",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/em_code+lib_k",
		"modules/src/print+lib",
		"modules/src/read_em+lib_ev",
		"modules/src/string+lib",
		"modules/src/system+lib",
		"modules/src/em_data+lib",
	}
}

cprogram {
	name = "decode",
	srcs = { "./convert.c" },
	deps = {
		"h+emheaders",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/em_code+lib_e",
		"modules/src/print+lib",
		"modules/src/read_em+lib_kv",
		"modules/src/string+lib",
		"modules/src/system+lib",
		"modules/src/em_data+lib",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_encode"] = "+encode",
		["$(PLATDEP)/em_decode"] = "+decode",
		["$(INSDIR)/bin/esize"] = "+esize",
		["$(INSDIR)/share/man/man1/esize.1"] = "./esize.1",
		["$(INSDIR)/share/man/man6/em_decode.6"] = "./em_decode.6"
	}
}
