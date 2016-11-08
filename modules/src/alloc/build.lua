clibrary {
	name = "lib",
	srcs = { "./*.c" },
	hdrs = { "./alloc.h" },
	deps = {
		"modules+headers",
		"modules/src/system+lib",
		"./*.h"
	},
}

