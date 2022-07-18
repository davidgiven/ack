bundle {
	name = "srcs",
	srcs = {
		"./close.c",
		"./creat.c",
		"./execve.c",
		"./_exit.c",
		"./fork.c",
		"./getpid.c",
		"./gettimeofday.c",
		"./_hol0.s",
		"./ioctl.c",
		"./isatty.c",
		"./kill.c",
		"./lseek.c",
		"./open.c",
		"./read.c",
		"./sbrk.c",
		"./signal.c",
		"./sigprocmask.c",
		"./unlink.c",
		"./wait.c",
		"./waitpid.c",
		"./write.c",
	}
}

bundle {
	name = "headers",
	srcs = {
		"./libsys.h",
		"./syscalls.h",
	}
}

