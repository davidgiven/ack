clibrary {
	name = "lib",
	srcs = {
		"./basename.c",
		"./filesize.c",
		"./maketempfile.c",
		"./setbinarymode.c",
		"./stop.c",
		"./strndup.c",
		"./syssystem.c",
		"./tmpdir.c",
	},
	hdrs = { "./system.h" },
	deps = {
		"modules/src/data+lib",
	}
}
