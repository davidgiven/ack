acklibrary {
    name = "lib",
    srcs = {
        "./*.c",
        "./*.s",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/pc86/include+headers",
	},
    vars = {
        plat = "pc86"
    }
}

