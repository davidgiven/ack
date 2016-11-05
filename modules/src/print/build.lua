clibrary {
	name = "lib",
	srcs = { "./*.c" },
	hdrs = { "./print.h" },
	deps = {
		"modules+headers",
		"modules/src/system+lib",
		"./*.h"
	}
}


