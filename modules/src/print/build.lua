clibrary {
	name = "lib",
	srcs = {
		"./doprnt.c", "./format.c", "./fprint.c", "./print.c",
		"./sprint.c",
	},
	hdrs = { "./print.h" },
	deps = {
		"./param.h",
		"modules/src/string+lib",
		"modules/src/system+lib",
	}
}
