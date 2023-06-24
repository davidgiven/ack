acklibrary {
    name = "lib",
    srcs = {
		"./_hol0.s",
		"./_sys_rawread.s",
		"./_sys_rawwrite.s",
		"./_trap.s",
		"./brk.c",
		"./close.c",
		"./creat.c",
		"./errno.s",
		"./getpid.c",
		"./isatty.c",
		"./kill.c",
		"./lseek.c",
		"./open.c",
		"./read.c",
		"./signal.c",
		"./time.c",
		"./write.c",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/agonmos/include+headers",
	},
    vars = {
        plat = "agonmos"
    }
}


