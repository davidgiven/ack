acklibrary {
    name = "lib",
    srcs = {
        "./*.e",
        "./*.c",
        "plat/em/libsys/*.c",
        "plat/em/libsys/*.e",
    },
	deps = {
		"plat/em/libsys/*.h",
		"lang/cem/libcc.ansi/headers+headers",
        "plat/em22/include+pkg",
		"h+emheaders",
	},
    vars = {
        plat = "em22"
    }
}

