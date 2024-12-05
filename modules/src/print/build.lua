clibrary {
	name = "lib",
	srcs = {
		"./doprnt.c", "./format.c", "./fprint.c",
	},
	hdrs = { "./print.h" },
	deps = {
		"./param.h",
		"modules/src/string+lib",
		"modules/src/system+lib",
	}
}
