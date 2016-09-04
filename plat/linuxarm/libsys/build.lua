acklibrary {
    name = "lib",
    srcs = {
        "./*.s",
        "plat/linux/libsys/*.c",
        "plat/linux/libsys/*.s",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/linuxarm/include+headers",
	},
    vars = {
        plat = "linuxarm"
    }
}

