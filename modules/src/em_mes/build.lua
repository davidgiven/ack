clibrary {
	name = "lib",
	srcs = { "./*.c" },
	deps = {
		"h+emheaders",
		"modules+headers",
		"modules/src/em_code+headers",
		"modules/src/em_data+lib",
	}
}

