acklibrary {
    name = "lib",
    srcs = {
		"./brk.c",
		"./close.c",
		"./creat.c",
		"./errno.s",
		"./getpid.c",
		"./_hol0.s",
		"./isatty.c",
		"./kill.c",
		"./lseek.c",
		"./open.c",
		"./pi_fast_mode.s",
		"./pi_phys_to_user.s",
		"./pi_uart.s",
		"./pi_user_to_phys.s",
		"./read.c",
		"./select.c",
		"./signal.c",
		"./tcgetattr.c",
		"./tcsetattr.c",
		"./time.c",
		"./write.c",
    },
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/rpi/include+headers",
	},
    vars = {
        plat = "rpi"
    }
}

