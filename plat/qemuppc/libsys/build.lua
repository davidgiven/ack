acklibrary {
    name = "lib",
    srcs = {
        "./*.s",
        "./*.c",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/qemuppc/include+headers",
        "mach/powerpc/libem+headers_qemuppc",
	},
    vars = {
        plat = "qemuppc"
    }
}
 