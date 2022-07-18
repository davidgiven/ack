acklibrary {
    name = "lib",
    srcs = {
		"./_syscall.s",
        "plat/linux/libsys+srcs",
    },
	deps = {
		"plat/linux/libsys+headers",
		"lang/cem/libcc.ansi/headers+headers",
		"plat/linux68k/include+pkg",
	},
    vars = {
        plat = "linux68k"
    }
}

