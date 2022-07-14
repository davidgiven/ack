clibrary {
	name = "lib",
	srcs = {
		"./botch.c",
		"./clear.c",
		"./Malloc.c",
		"./No_Mem.c",
		"./Realloc.c",
		"./Salloc.c",
		"./Srealloc.c",
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

