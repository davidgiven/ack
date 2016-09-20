include("first/yacc.lua")

flex {
	name = "flex",
	srcs = { "./*.l" },
}

yacc {
	name = "yacc",
	srcs = { "./*.y" },
}

cprogram {
	name = "mcgg",
	srcs = {
		"./*.c",
		matching(filenamesof("+flex"), "%.c$"),
		matching(filenamesof("+yacc"), "%.c$")
	},
	deps = {
		"./*.h",
		"+yacc"
	}
}

