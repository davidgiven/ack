acklibrary {
    name = "lib",
    srcs = {
        "./*.c",
        "./*.s",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/msdos86/include+headers",
	},
    vars = {
        plat = "msdos86"
    }
}

