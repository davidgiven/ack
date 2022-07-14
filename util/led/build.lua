cprogram {
    name = "led",
    srcs = {
		"./archive.c",
		"./error.c",
		"./extract.c",
		"./finish.c",
		"./main.c",
		"./memory.c",
		"./output.c",
		"./read.c",
		"./relocate.c",
		"./save.c",
		"./scan.c",
		"./sym.c",
		"./write.c",
	},
    deps = {
        "./archive.h", "./const.h", "./debug.h", "./defs.h", 
        "./error.h", "./extract.h", "./finish.h", "./mach.h",
        "./memory.h", "./orig.h", "./output.h", "./relocate.h",
        "./save.h", "./scan.h", "./sym.h", "./write.h",
        "modules/src/string+lib",
        "modules/src/object+lib",
        "h+emheaders",
    }
}

installable {
    name = "pkg",
    map = {
        ["$(INSDIR)/share/man/man5/ack.out.5"] = "./ack.out.5",
        ["$(INSDIR)/share/man/man6/led.6"] = "./led.6",
        ["$(PLATDEP)/em_led"] = "+led",
    }
}

