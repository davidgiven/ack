bundle {
	name = "srcs",
	srcs = {
		"./creat.c",
		"./gettimeofday.c",
		"./kill.c",
		"./lseek.c",
		"./open.c",
		"./read.c",
		"./setmode.c",
		"./signal.c",
		"./sys_fdmodes.c",
		"./sys_getmode.c",
		"./sys_initmain.c",
		"./sys_iseof.c",
		"./sys_seteof.c",
		"./sys_seterrno.c",
		"./sys_setmode.c",
		"./write.c",
	}
}

bundle {
	name = "headers",
	srcs = {
		"./libsys.h"
	}
}
