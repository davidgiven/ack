bundle {
	name = "headers",
	srcs = { "./h/*.h" }
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

