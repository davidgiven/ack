clibrary {
	name = "lib",
	srcs = {
		"./access.c",
		"./basename.c",
		"./close.c",
		"./create.c",
		"./filesize.c",
		"./maketempfile.c",
		"./modtime.c",
		"./open.c",
		"./read.c",
		"./rename.c",
		"./seek.c",
		"./setbinarymode.c",
		"./stop.c",
		"./strndup.c",
		"./syssystem.c",
		"./system.c",
		"./tmpdir.c",
		"./write.c",
		--"./lock.c",
		--"./unlock.c”,
	},
	hdrs = { "./system.h" },
	deps = {
		"modules/src/data+lib",
	}
}
