clibrary {
	name = "lib",
	srcs = { "./*.c" },
	deps = {
		"h+emheaders",
		"modules+headers",
		"modules/src/em_code+em_code_ek_h",
		"util/data+em_data",
	}
}

