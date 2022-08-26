bundle {
	name = "headers",
	srcs = {
		"./libsysasm.h"
	}
}

acklibrary {
	name = "lib",
	srcs = {
		"./_hol0.s",
		"./brk.s",
		"./close.s",
		"./errno.s",
		"./getpid.s",
		"./isatty.s",
		"./rename.s",
		"./sbrk.c",
		"./sys_cpyin.s",
		"./sys_cpyout.s",
		"./sys_dpmidos.s",
		"./sys_exists.s",
		"./sys_getdate.s",
		"./sys_gettime.s",
		"./sys_isopen.s",
		"./sys_isreadyr.s",
		"./sys_rawcreat.s",
		"./sys_rawlseek.s",
		"./sys_rawopen.s",
		"./sys_rawread.s",
		"./sys_rawwrite.s",
		"./sys_scpyout.s",
		"./sys_xret.s",
		"./unlink.s",
		"plat/msdos/libsys+srcs",
	},
	deps = {
		"plat/msdos/libsys+headers",
		"+headers"
	},
	vars = {
		plat = "msdos386"
	}
}
