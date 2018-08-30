acklibrary {
    name = "lib",
    srcs = {
        "./*.s",
        "plat/linux/libsys/*.c",
        "plat/linux/libsys/*.s",
    },
	deps = {
		"plat/linux/libsys/*.h",
		"lang/cem/libcc.ansi/headers+headers",
		"plat/linux386/include+pkg",
	},
    vars = {
        plat = "linux386"
    }
}

