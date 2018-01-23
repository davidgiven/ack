acklibrary {
	name = "lib",
	srcs = {
		"./_exit.s",
		"./close.s",
		"./fstat.s",
		"./getdirentries.s",
		"./getpid.s",
		"./gettimeofday.s",
		"./ioctl.s",
		"./kill.s",
		"./lseek.s",
		"./lstat.s",
		"./mmap.s",
		"./mprotect.s",
		"./open.s",
		"./read.s",
		"./set_errno.s",
		"./sigaction.s",
		"./stat.s",
		"./write.s",
		"plat/linux/libsys/_hol0.s",
		"plat/linux386/libsys/trapno.s",
		"plat/osx/libsys/brk.c",
		"plat/osx/libsys/creat.c",
		"plat/osx/libsys/isatty.c",
		"plat/osx/libsys/signal.c",
	},
	deps = {
		"lang/cem/libcc.ansi/headers+headers",
		"plat/osx386/include+pkg",
	},
	vars = {
		plat = "osx386"
	}
}
