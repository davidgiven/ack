acklibrary {
	name = "lib",
	srcs = {
		"./_exit.s",
		"./close.s",
		"./getpid.s",
		"./gettimeofday.s",
		"./ioctl.s",
		"./kill.s",
		"./lseek.s",
		"./mmap.s",
		"./mprotect.s",
		"./open.s",
		"./read.s",
		"./set_errno.s",
		"./sigaction.s",
		"./write.s",
		"plat/linuxppc/libsys/trap.s",
		"plat/osx/libsys/brk.c",
		"plat/osx/libsys/creat.c",
		"plat/osx/libsys/isatty.c",
		"plat/osx/libsys/signal.c",
	},
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/osxppc/include+headers",
	},
	vars = {
		plat = "osxppc"
	}
}
