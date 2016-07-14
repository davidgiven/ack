bundle {
	name = "headers",
	srcs = { "./h/*.h" }
}

clibrary {
	name = "system",
	srcs = { "./src/system/*.c" },
	hdrs = { "./src/system/system.h" },
	deps = { "+headers" },
}

clibrary {
	name = "alloc",
	srcs = { "./src/alloc/*.c" },
	hdrs = { "./src/alloc/alloc.h" },
	deps = {
		"+headers",
		"+system"
	},
}

clibrary {
	name = "flt_arith",
	srcs = { "./src/flt_arith/*.c" },
	hdrs = { "./src/flt_arith/flt_arith.h" },
	deps = {
		"+headers"
	}
}

clibrary {
	name = "idf",
	srcs = {},
	hdrs = { "./src/idf/idf_pkg.*" },
}

clibrary {
	name = "string",
	srcs = { "./src/string/*.c" },
	deps = { "+headers" },
}

clibrary {
	name = "object",
	srcs = { "./src/object/*.c" },
	deps = {
		"+headers",
		"h+local",
		"h+emheaders",
	},
}

