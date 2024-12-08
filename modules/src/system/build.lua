clibrary {
	name = "lib",
	srcs = {
		"./basename.c",
		"./close.c",
		"./create.c",
		"./filesize.c",
		"./maketempfile.c",
		"./open.c",
		"./read.c",
		"./seek.c",
		"./setbinarymode.c",
		"./stop.c",
		"./strndup.c",
		"./syssystem.c",
		"./system.c",
		"./tmpdir.c",
		"./write.c",
	},
	hdrs = { "./system.h" },
	deps = {
		"modules/src/data+lib",
	}
}
