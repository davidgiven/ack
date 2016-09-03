acklibrary {
    name = "lib",
    srcs = {
        "./*.c",
        "./*.s",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/rpi/include+headers",
	},
    vars = {
        plat = "rpi"
    }
}

