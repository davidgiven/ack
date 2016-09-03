acklibrary {
    name = "lib",
    srcs = {
        "./*.c",
        "./*.s",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/cpm/include+headers",
	},
    vars = {
        plat = "cpm"
    }
}

