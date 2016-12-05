clibrary {
	name = "lib",
	srcs = { "./*.c" },
	hdrs = { "./object.h" },
	deps = {
		"modules+headers",
		"h+local",
		"h+emheaders",
		"./*.h",
	},
}


