acklibrary {
    name = "lib",
    srcs = {
		"./_hol0.s",
		"./errno.s",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/bbcmicro/include+headers",
	},
    vars = {
        plat = "bbcmicro"
    }
}

