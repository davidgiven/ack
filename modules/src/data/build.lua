local hfiles = {
	"./array.h",
	"./astring.h",
	"./diagnostics.h",
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
		"./diagnostics.c",
		"./imap.c",
		"./pmap.c",
		"./smap.c",
		"./stringlist.c",
	},
	hdrs = hfiles,
	deps = hfiles,
}

