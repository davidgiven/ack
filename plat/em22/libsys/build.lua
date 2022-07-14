acklibrary {
    name = "lib",
    srcs = {
        "plat/em/libsys+srcs",
    },
	deps = {
		"plat/em/libsys+headers",
		"lang/cem/libcc.ansi/headers+headers",
        "plat/em22/include+pkg",
		"h+emheaders",
	},
    vars = {
        plat = "em22"
    }
}

