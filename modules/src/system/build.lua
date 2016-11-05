clibrary {
	name = "lib",
	srcs = { "./*.c" },
	hdrs = { "./system.h" },
	deps = {
		"modules+headers",
		"./*.h"
	},
}


