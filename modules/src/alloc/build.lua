clibrary {
	name = "lib",
	srcs = {
		"./Salloc.c",
		"./st_alloc.c",
		"./std_alloc.c",
	},
	hdrs = { "./alloc.h" },
	deps = {
		"modules+headers",
		"modules/src/system+lib",
		"./alloc.h"
	},
}

