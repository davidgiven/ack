local hfiles = {
	"./array.h",
	"./astring.h",
	"./warnings.h",
	"./imap.h",
	"./pmap.h",
	"./smap.h",
	"./stringlist.h",
}

clibrary {
	name = "lib",
	srcs = {
		"./array.c",
		"./astring.c",
		"./warnings.c",
		"./imap.c",
		"./pmap.c",
		"./smap.c",
		"./stringlist.c",
	},
	hdrs = hfiles,
	deps = hfiles,
}

