clibrary {
	name = "lib",
	srcs = { "./*.c" },
	deps = {
		"modules+headers",
		"h+local",
		"h+emheaders",
	},
}


