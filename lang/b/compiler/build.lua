
cprogram {
	name = "em_b",
	srcs = {
		"./*.c",
	},
	deps = {
		"modules+headers",
		"modules/src/em_code+lib_k",
		"modules/src/em_data+lib",
		"modules/src/em_mes+lib",
	}
}

installable {
	name = "pkg",
	map = {
		["$(PLATDEP)/em_b"] = "+em_b"
	}
}

