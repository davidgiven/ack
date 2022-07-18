acklibrary {
    name = "lib",
    srcs = {
		"./execl.s",
		"./_syscall.s",
		"./trapno.s",
        "plat/linux/libsys+srcs",
    },
	deps = {
		"plat/linux/libsys+headers",
		"lang/cem/libcc.ansi/headers+headers",
		"plat/linux386/include+pkg",
	},
    vars = {
        plat = "linux386"
    }
}

