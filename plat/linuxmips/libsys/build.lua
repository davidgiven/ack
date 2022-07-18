acklibrary {
	name = "lib",
	srcs = {
		"./_syscall.s",
		"plat/linux/libsys/_exit.c",
		"plat/linux/libsys/_hol0.s",
		"plat/linux/libsys/close.c",
		"plat/linux/libsys/creat.c",
		"plat/linux/libsys/execve.c",
		"plat/linux/libsys/fork.c",
		"plat/linux/libsys/getpid.c",
		"plat/linux/libsys/gettimeofday.c",
		"plat/linux/libsys/ioctl.c",
		"plat/linux/libsys/isatty.c",
		"plat/linux/libsys/kill.c",
		"plat/linux/libsys/lseek.c",
		"plat/linux/libsys/open.c",
		"plat/linux/libsys/read.c",
		"plat/linux/libsys/sbrk.c",
		"plat/linux/libsys/signal.c",
		"plat/linux/libsys/sigprocmask.c",
		"plat/linux/libsys/unlink.c",
		"plat/linux/libsys/wait.c",
		"plat/linux/libsys/waitpid.c",
		"plat/linux/libsys/write.c",
	},
	deps = {
		"plat/linux/libsys+headers",
		"lang/cem/libcc.ansi/headers+headers",
		"plat/linuxmips/include+pkg",
	},
	vars = {
		plat = "linuxmips"
	}
}

