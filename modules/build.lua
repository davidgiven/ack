bundle {
	name = "headers",
	srcs = { "./h/*.h" }
}

clibrary {
	name = "string",
	srcs = { "./src/string/*.c" },
	deps = { ":headers" },
}

