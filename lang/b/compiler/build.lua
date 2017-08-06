
cprogram {
	name = "em_b",
	srcs = {
		"./*.c",
	},
	deps = {
		"./*.h",
		"modules+headers",
		"modules/src/alloc+lib",
		"modules/src/data+lib",
		"modules/src/em_code+lib_k",
		"modules/src/em_data+lib",
		"modules/src/em_mes+lib",
		"modules/src/print+lib",
		"modules/src/string+lib",
		"modules/src/system+lib",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_b"] = "+em_b",
		["$(INSDIR)/share/man/man6/em_b.6"] = "./em_b.6"
	}
}

